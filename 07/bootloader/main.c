#include "defines.h"
#include "lib.h"
#include "serial.h"
#include "xmodem.h"
#include "elf.h"
#include "interrupt.h"

static int init(void) {
	/*ld.scrで定義したシンボル*/
	extern int erodata, data_start, edata, bss_start, ebss;

	/*data領域とbss領域を初期化する*/
	/*これ以降で静的領域の変数を利用可能となる*/
	memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
	memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

	/*ソフトウェア割り込みベクタの初期化*/
	softvec_init();

	/*シリアルの初期化*/
	serial_init(SERIAL_DEFAULT_DEVICE);

	return 0;
}

// メモリの16進ダンプ出力
static int dump(char *buf, long size) {
	long i;

	if (size < 0) {
		puts("no data.\n");
		return -1;
	}
	for (i = 0; i < size; i++) {
		putxval(buf[i], 2);
		if ((i & 0xf) == 15) {
			puts("\n");
		} else {
			if ((i & 0xf) == 7) puts(" ");
			puts(" ");
		}
	}
	puts("\n");

	return 0;
}

static void wait() {
	volatile long i;
	for (i = 0; i < 300000; i++)
		;
}

int main(void) {
	static char buf[16];
	static long size = -1;
	static unsigned char *loadbuf = NULL;
	extern int buffer_start;  // リンカ・スクリプトで定義されているバッファ
	char *entry_point;
	ep f;

	// 冒頭の初期化は割り込み無効
	INTR_DISABLE;
	init();

	puts("kzload (kozos boot loader) started.\n");

	while (1) {
		puts("kzload> ");  // プロンプト表示
		gets(buf);         // シリアルからのコマンド受信

		if (!strcmp(buf, "load")) {  // XMODEMでのファイルダウンロード
			loadbuf = (char *)(&buffer_start);
			size = xmodem_recv(loadbuf);
			wait();  // 転送アプリが終了し端末アプリに制御が戻るまで待ち合わせる
			if (size < 0) {
				puts("\nXMODEM receive error!\n");
			} else {
				puts("\nXMODEM receive succeeded.\n");
			}
		} else if (!strcmp(buf, "dump")) {  // メモリの16進ダンプ出力
			puts("size: ");
			putxval(size, 0);
			puts("\n");
			dump(loadbuf, size);
		} else if (!strcmp(buf, "run")) {	// メモリ上に展開(ロード)
			entry_point = elf_load(loadbuf);
			if (!entry_point) {
				puts("run error!\n");
			}else{
				puts("starting from entry point: ");
				putxval((unsigned long) entry_point, 0);
				puts("\n");
				// ロードしたosに処理を渡す
				f = (ep) entry_point;
				f();
			}
		} else {
			puts("unknown.\n");
		}
	}

	return 0;
}
