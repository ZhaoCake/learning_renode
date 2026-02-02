OUTPUT_ARCH( "riscv" )
ENTRY(_start)

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

  . = ALIGN(8);
  _stack_top = ORIGIN(ram) + LENGTH(ram);
}
