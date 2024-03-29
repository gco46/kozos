#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "lib.h"

#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15

// static関数 プロトタイプ宣言
static void thread_intr(softvec_type_t type, unsigned long sp);


/* スレッド・コンテキスト */
typedef struct _kz_context {
	uint32 sp;	/* スタックポインタ */
} kz_context;

/* タスク・コントロール・ブロック(TCB) */
typedef struct _kz_thread {
	struct _kz_thread *next;	/* レディー・キューへの接続に利用するポインタ */
	char name[THREAD_NAME_SIZE + 1];
	char *stack;

	struct {	/* スレッドのスタート・アップ(thread_init())に渡すパラメータ */
		kz_func_t func;		/*スレッドのメイン関数*/
		int argc;		/* スレッドのメイン関数に渡す argc */
		char **argv;	/* スレッドのメイン関数に渡す argv */
	} init;

	struct {	/* システム・コール用バッファ */
		kz_syscall_type_t type;
		kz_syscall_param_t *param;
	} syscall;

	kz_context context;
} kz_thread;

/* スレッドのレディ・キュー */
static struct {
	kz_thread *head;
	kz_thread *tail;
} readyqueue;

static kz_thread *current;	/* カレント・スレッド */
static kz_thread threads[THREAD_NUM]; /* タスク・コントロール・ブロック */
static kz_handler_t handlers[SOFTVEC_TYPE_NUM];	/* 割り込みハンドラ */

void dispatch(kz_context *context);

/* カレント・スレッドをレディ・キューから抜き出す */
static int getcurrent(void) {
	if (current == NULL) {
		return -1;
	}

	readyqueue.head = current->next;
	if (readyqueue.head == NULL) {	/* スレッドが1つの場合 */
		readyqueue.tail = NULL;
	}
	current->next = NULL;

	return 0;
}

/* レディ・キューの末尾にカレント・スレッドを接続する */
static int putcurrent(void) {
	if (current == NULL) {
		return -1;
	}

	if (readyqueue.tail) {
		readyqueue.tail->next = current;
	} else {	/* スレッドが1つの場合 */
		readyqueue.head = current;
	}
	readyqueue.tail = current;

	return 0;
}

static void thread_end(void) {
	kz_exit();
}

static void thread_init(kz_thread *thp) {
	/* スレッドのメイン関数を呼び出す */
	thp->init.func(thp->init.argc, thp->init.argv);
	thread_end();
}

/* システム・コールの処理(kz_run()) スレッドの起動 */
static kz_thread_id_t thread_run(
	kz_func_t func,
	char *name,
	int stacksize,
	int argc,
	char *argv[])
{
	int i;
	kz_thread *thp;
	uint32 *sp;
	extern char userstack;	/* リンカ・スクリプトで定義されるスタック領域 */
	static char *thread_stack = &userstack;

	/* 空いているタスク・コントロール・ブロックを検索 */
	for (i = 0; i < THREAD_NUM; i++) {
		thp = &threads[i];
		if (!thp->init.func) {
			break;
		}
	}
	if (i == THREAD_NUM) {
		return -1;
	}

	/* TCBを0クリアする */
	memset(thp, 0, sizeof(*thp));

	/* TCBの設定 */
	strcpy(thp->name, name);
	thp->next = NULL;

	thp->init.func = func;
	thp->init.argc = argc;
	thp->init.argv = argv;

	/* スタック領域を獲得 */
	memset(thread_stack, 0, stacksize);
	thread_stack += stacksize;

	thp->stack = thread_stack;

	/* スタックの初期化 */
	sp = (uint32 *)thp->stack;
	*(--sp) = (uint32)thread_end;	/* thread_init()からの戻り先としてthread_end()を登録 */

	/* プログラム・カウンタを設定する */
	*(--sp) = (uint32)thread_init;	/* ディスパッチ時にプログラム・カウンタに格納される値を設定 */
									/* →thread_init()から動作を開始 */
	*(--sp) = 0;	/* ER6 */
	*(--sp) = 0;	/* ER5 */
	*(--sp) = 0;	/* ER4 */
	*(--sp) = 0;    /* ER3 */
	*(--sp) = 0;    /* ER2 */
	*(--sp) = 0;    /* ER1 */

	/* スレッドのスタート・アップ(thread_init())に渡す引数 */
	*(--sp) = (uint32)thp;	/* ER0 */

	/* スレッドのコンテキストを設定 */
	thp->context.sp = (uint32)sp;

	/* システム・コールを呼び出したスレッドをレディ・キューに戻す */
	putcurrent();

	/* 新規作成したスレッドを, レディ・キューに接続する */
	current = thp;
	putcurrent();

	return (kz_thread_id_t)current;
}

/* システム・コールの処理(kz_exit()) スレッドの終了 */
static int thread_exit(void) {
	puts(current->name);
	puts(" EXIT.\n");
	memset(current, 0, sizeof(*current));
	return 0;
}

/* 割り込みハンドラの登録 */
static int setintr(softvec_type_t type, kz_handler_t handler){

	softvec_setintr(type, thread_intr);

	handlers[type] = handler;	/* OS側から呼び出す割り込みハンドラを登録 */

	return 0;
}

static void call_functions(kz_syscall_type_t type, kz_syscall_param_t *p) {
	/* システム・コールの実行中にcurrentが書き換わるので注意 */
	switch(type) {
		case KZ_SYSCALL_TYPE_RUN:	/* kz_run() */
			p->un.run.ret = thread_run(p->un.run.func,
									   p->un.run.name,
									   p->un.run.stacksize,
									   p->un.run.argc,
									   p->un.run.argv);
			break;
		case KZ_SYSCALL_TYPE_EXIT:	/* kz_exit() */
			/* TCBが消去されるので、値を書き込んではいけない */
			thread_exit();
			break;
		default:
			break;
	}
}

/* システムコールの処理 */
static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t *p) {
	getcurrent();
	call_functions(type, p);
}

/* スレッドのスケジューリング */
static void schedule(void) {
	if (!readyqueue.head) {		/* 見つからなかった */
		kz_sysdown();
	}
	current = readyqueue.head;
}

static void syscall_intr(void){
	syscall_proc(current->syscall.type, current->syscall.param);
}

static void softerr_intr(void){
	puts(current->name);
	puts(" DONW.\n");
	getcurrent();
	thread_exit();
}

/* 割り込み処理の入り口関数 */
static void thread_intr(softvec_type_t type, unsigned long sp){
	/* カレントスレッドのコンテキストを保存する */
	current->context.sp = sp;

	/* 割り込みtype毎の処理を実行する */
	if (handlers[type]){
		handlers[type]();
	}
	schedule();

	/* スレッドのディスパッチ */
	dispatch(&current->context);
	/*ディスパッチ後はそのスレッドの動作に入るので、dispatch()は返ってこない*/
}

void kz_start(kz_func_t func, char *name, int statcksize, int argc, char *argv[]){
	/* currentをNULLで初期化 */
	current = NULL;

	readyqueue.head = NULL;
	readyqueue.tail = NULL;
	memset(threads, 0, sizeof(threads));
	memset(handlers, 0, sizeof(handlers));

	/* 割り込みハンドラの登録 */
	setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
	setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

	/* システムコール発行不可なので直接関数を呼び出してスレッド作成する */
	current = (kz_thread *)thread_run(func, name, statcksize, argc, argv);

	/* 最初のスレッドを起動 */
	dispatch(&current->context);

	/* ここには返ってこない */
}

void kz_sysdown(void){
	puts("system error!\n");
	while(1)
		;
}

/* システムコール呼び出し用ライブラリ関数 */
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param){
	current->syscall.type = type;
	current->syscall.param = param;
	asm volatile ("trapa #0");	/* トラップ割り込み発行 */
}

