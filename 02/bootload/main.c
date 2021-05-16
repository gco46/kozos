#include "defines.h"
#include "lib.h"
#include "serial.h"

int main(void) {
    serial_init(SERIAL_DEFAULT_DEVICE);
    puts("Hello World!\n");

    putxval(0x10, 0);
    puts("\n");
    putxval(0xffff, 0);
    puts("\n");

    while (1)
        ;

    return 0;
}
