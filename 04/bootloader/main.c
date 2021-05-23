#include "defines.h"
#include "lib.h"
#include "serial.h"

int global_data = 0x10;
int global_bss;
static int static_data = 0x20;
static int static_bss;

static void printval(void) {
    puts("global_data = ");
    putxval(global_data, 0);
    puts("\n");

    puts("global_bss = ");
    putxval(global_bss, 0);
    puts("\n");

    puts("static_data = ");
    putxval(static_data, 0);
    puts("\n");

    puts("static_bss = ");
    putxval(static_bss, 0);
    puts("\n");
}

static int init(void) {
    /*ld.scrで定義したシンボル*/
    extern int erodata, data_start, edata, bss_start, ebss;

    /*data領域とbss領域を初期化する*/
    /*これ以降で静的領域の変数を利用可能となる*/
    memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
    memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

    /*シリアルの初期化*/
    serial_init(SERIAL_DEFAULT_DEVICE);

    return 0;
}

int main(void) {
    init();
    puts("Hello World!\n");

    puts("overwirte variables.\n");
    global_data = 0x20;
    global_bss = 0x30;
    static_data = 0x40;
    static_bss = 0x50;
    printval();

    while (1)
        ;

    return 0;
}
