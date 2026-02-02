.section .init, "ax"
.global _start
.global trap_entry

_start:
    /* Disable Interrupts (clear MIE) */
    csrr t0, mstatus
    li t1, 0x8
    not t1, t1
    and t0, t0, t1
    csrw mstatus, t0

    /* Initialize stack */
    la sp, _stack_top

    /* Set trap vector */
    la t0, trap_entry
    csrw mtvec, t0

    /* Copy .data */
    la a0, _sdata
    la a1, _edata
    la a2, _etext
    bge a0, a1, end_copy_data
copy_data:
    lw t0, 0(a2)
    sw t0, 0(a0)
    addi a0, a0, 4
    addi a2, a2, 4
    blt a0, a1, copy_data
end_copy_data:

    /* Zero .bss */
    la a0, _sbss
    la a1, _ebss
    bge a0, a1, end_zero_bss
    li t0, 0
zero_bss:
    sw t0, 0(a0)
    addi a0, a0, 4
    blt a0, a1, zero_bss
end_zero_bss:

    call main

hang:
    j hang

/* Trap entry: save caller-saved registers and call C handler */
    .align 2
trap_entry:
    addi sp, sp, -64
    sw ra, 0(sp)
    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t2, 12(sp)
    sw t3, 16(sp)
    sw t4, 20(sp)
    sw t5, 24(sp)
    sw t6, 28(sp)
    sw a0, 32(sp)
    sw a1, 36(sp)
    sw a2, 40(sp)
    sw a3, 44(sp)
    sw a4, 48(sp)
    sw a5, 52(sp)
    sw a6, 56(sp)
    sw a7, 60(sp)

    call trap_handler_c

    lw ra, 0(sp)
    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t2, 12(sp)
    lw t3, 16(sp)
    lw t4, 20(sp)
    lw t5, 24(sp)
    lw t6, 28(sp)
    lw a0, 32(sp)
    lw a1, 36(sp)
    lw a2, 40(sp)
    lw a3, 44(sp)
    lw a4, 48(sp)
    lw a5, 52(sp)
    lw a6, 56(sp)
    lw a7, 60(sp)
    addi sp, sp, 64

    mret
