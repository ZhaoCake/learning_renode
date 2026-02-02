
.section .text, "ax"
.global _start

# BootROM at 0x00001000
# Jump to flash base 0x20000000 where firmware.bin is loaded.
_start:
    lui t0, 0x20000         # t0 = 0x2000_0000
    jalr zero, 0(t0)        # jump to firmware entry (_start)

1:
    j 1b
