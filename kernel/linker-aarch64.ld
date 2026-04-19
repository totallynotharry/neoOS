ENTRY(neo_kernel_main)

SECTIONS
{
  . = 0x40080000;

  .text : ALIGN(4K) {
    *(.text*)
  }

  .rodata : ALIGN(4K) {
    *(.rodata*)
  }

  .data : ALIGN(4K) {
    *(.data*)
  }

  .bss : ALIGN(4K) {
    *(COMMON)
    *(.bss*)
  }
}
