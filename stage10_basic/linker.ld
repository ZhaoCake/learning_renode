OUTPUT_ARCH( "riscv" )
ENTRY(_start)

/* FE310-G000 Memory Map */
/* Flash: 0x20000000 (usually mapped to 0x20400000 for code in some contexts, but standard is XIP at 0x20000000) */
/* DTIM (RAM): 0x80000000, Length 16KB */

MEMORY
{
  flash (rxai!w) : ORIGIN = 0x20000000, LENGTH = 512M
  ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 16K
}

SECTIONS
{
  .init :
  {
    KEEP (*(.init))
  } > flash

  .text :
  {
    *(.text .text.*)
    *(.rodata .rodata.*)
    . = ALIGN(4);
    _etext = .;
  } > flash

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data .data.*)
    *(.sdata .sdata.*)
    . = ALIGN(4);
    _edata = .;
  } > ram AT > flash

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss .bss.*)
    *(.sbss .sbss.*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > ram

  /* Stack Top */
  . = ALIGN(8);
  _stack_top = ORIGIN(ram) + LENGTH(ram);
}
