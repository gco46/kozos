#ifndef _KOZOS_H_INCLUDED_
#define _KOZOS_H_INCLUDED_

#include "defines.h"
#include "syscall.h"

/* システムコール */
kz_thread_id_t kz_run(			/* スレッド起動 */
	kz_func_t func,
	char *name,
	int stacksize,
	int argc,
	char *argv[]
);
void kz_exit(void);				/* スレッド終了 */

/* ライブラリ関数 */
void kz_start(					/* 初期スレッド起動, OS動作開始 */
	kz_func_t func,
	char *name,
	int stacksize,
	int argc,
	char *argv[]
);
void kz_sysdown(void);			/* 致命的エラーの通知 */
void kz_syscall(				/* システムコール実行 */
	kz_syscall_type_t type,
	kz_syscall_param_t *param
);

/* ユーザ・スレッド */
int test08_1_main(int argc, char *argv[]);		/* ユーザスレッドのメイン（テスト用） */

#endif
