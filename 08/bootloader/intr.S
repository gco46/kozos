#include "intr.h"
    .h8300h
    .section .text

    .global _intr_softerr
#   .type _intr_softerr, @fuction
_intr_softerr:
#   汎用レジスタの値をスタックに退避(er7がスタックポインタ)
    mov.l   er6,@-er7
    mov.l   er5,@-er7
    mov.l   er4,@-er7
    mov.l   er3,@-er7
    mov.l   er2,@-er7
    mov.l   er1,@-er7
    mov.l   er0,@-er7
    mov.l   er7,er1
    # 割り込みスタックの利用
    mov.l   #_intrstack,sp
    mov.l   er1,@-er7
    mov.w   #SOFTVEC_TYPE_SOFTERR,r0
    jsr     @_interrupt
    # 中断前のスタックポインタの値を割り込みスタックから復旧
    mov.l   @er7+,er1
    mov.l   @er1,er7
    # 退避した汎用レジスタの値を復元
    mov.l   @er7+,er0
    mov.l   @er7+,er1
    mov.l   @er7+,er2
    mov.l   @er7+,er3
    mov.l   @er7+,er4
    mov.l   @er7+,er5
    mov.l   @er7+,er6
    rte

    .global _intr_syscall
#   .type _intr_syscall,@function
_intr_syscall:
    mov.l   er6,@-er7
    mov.l   er5,@-er7
    mov.l   er4,@-er7
    mov.l   er3,@-er7
    mov.l   er2,@-er7
    mov.l   er1,@-er7
    mov.l   er0,@-er7
    mov.l   er7,er1
    # 割り込みスタックの利用
    mov.l   #_intrstack,sp
    mov.l   er1,@-er7
    mov.w   #SOFTVEC_TYPE_SYSCALL,r0
    jsr     @_interrupt
    # 中断前のスタックポインタの値を割り込みスタックから復旧
    mov.l   @er7+,er1
    mov.l   @er1,er7
    # 退避した汎用レジスタの値を復元
    mov.l   @er7+,er0
    mov.l   @er7+,er1
    mov.l   @er7+,er2
    mov.l   @er7+,er3
    mov.l   @er7+,er4
    mov.l   @er7+,er5
    mov.l   @er7+,er6
    rte

    .global _intr_serintr
#   .type _intr_serintr,@function
_intr_serintr:
    mov.l   er6,@-er7
    mov.l   er5,@-er7
    mov.l   er4,@-er7
    mov.l   er3,@-er7
    mov.l   er2,@-er7
    mov.l   er1,@-er7
    mov.l   er0,@-er7
    mov.l   er7,er1
    # 割り込みスタックの利用
    mov.l   #_intrstack,sp
    mov.l   er1,@-er7
    mov.w   #SOFTVEC_TYPE_SERINTR,r0
    jsr     @_interrupt
    # 中断前のスタックポインタの値を割り込みスタックから復旧
    mov.l   @er7+,er1
    mov.l   @er1,er7
    # 退避した汎用レジスタの値を復元
    mov.l   @er7+,er0
    mov.l   @er7+,er1
    mov.l   @er7+,er2
    mov.l   @er7+,er3
    mov.l   @er7+,er4
    mov.l   @er7+,er5
    mov.l   @er7+,er6
    rte

