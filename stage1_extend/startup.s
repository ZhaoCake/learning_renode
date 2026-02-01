.section .init, "ax"
.global _start

_start:
    /* Disable Interrupts (MIE bit in mstatus) */
    csrr t0, mstatus
    li t1, 0x8     /* MIE is bit 3 */
    not t1, t1
    and t0, t0, t1
    csrw mstatus, t0

    /* Initialize Stack Pointer */
    la sp, _stack_top

    /* Copy data segment */
    la a0, _sdata
    la a1, _edata
    la a2, _etext
    bge a0, a1, end_copy_data
loop_copy_data:
    lw t0, 0(a2)
    sw t0, 0(a0)
    addi a0, a0, 4
    addi a2, a2, 4
    blt a0, a1, loop_copy_data
end_copy_data:

    /* Zero fill bss segment */
    la a0, _sbss
    la a1, _ebss
    bge a0, a1, end_fill_bss
    li t0, 0
loop_fill_bss:
    sw t0, 0(a0)
    addi a0, a0, 4
    blt a0, a1, loop_fill_bss
end_fill_bss:

    /* Call main */
    call main

loop_forever:
    j loop_forever
