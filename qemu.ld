/*
 * A generic linker script for running on QEMU.
 * Assumes code starts at address 0x00000000 (Flash).
 * Assumes RAM starts at address 0x20000000.
 */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Define entry point */
ENTRY(Reset_Handler)

SECTIONS
{
    /* The .text section contains the program code and interrupt vector table */
    .text :
    {
        . = ALIGN(4);
        _s_text = .;
        KEEP(*(.isr_vector)) /* Keep the vector table at the beginning */
        *(.text)             /* All other .text sections */
        *(.text.*)
        *(.rodata)           /* Read-only data */
        *(.rodata.*)
        . = ALIGN(4);
        _e_text = .;
    } >FLASH

    /* .data section for initialized data */
    .data :
    {
        . = ALIGN(4);
        _s_data = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _e_data = .;
    } >RAM AT> FLASH /* Load from FLASH, run from RAM */
    _la_data = LOADADDR(.data);

    /* .bss section for uninitialized data */
    .bss :
    {
        . = ALIGN(4);
        _s_bss = .;
        __bss_start__ = _s_bss;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _e_bss = .;
        __bss_end__ = _e_bss;
    } >RAM

    /* Minimum stack size */
    _min_stack_size = 0x400;
}
