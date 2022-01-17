#ifndef _DEFINES_H_INCLUDED_
#define _DEFINES_H_INCLUDED_

#define NULL ((void *)0)
// 標準のシリアルデバイス
#define SERIAL_DEFAULT_DEVICE 1

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

// エントリポイント関数ポインタ
typedef void (*ep)(void);

#endif
