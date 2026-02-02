OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
  bootrom (rx) : ORIGIN = 0x00001000, LENGTH = 4K
}

SECTIONS
{
  .text :
  {
    *(.text .text.*)
    *(.rodata .rodata.*)
    . = ALIGN(4);
  } > bootrom
}
