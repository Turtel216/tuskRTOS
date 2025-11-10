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

/* Define the top of the stack. It's the end of the RAM region. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

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

    /*
     * The .data section contains initialized data.
     * It is loaded into RAM from FLASH at startup.
     * The '_la_data' symbol must be defined *inside* this block.
     */
    .data : AT ( _e_text )
    {
        . = ALIGN(4);
        _s_data = .;         /* Create a symbol for the start of .data */
        _la_data = LOADADDR(.data); /* Defines the load address symbol */
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _e_data = .;         /* Create a symbol for the end of .data */
    } >RAM

    /* .bss section for uninitialized data (zero-initialized at startup) */
    .bss :
    {
        . = ALIGN(4);
        _s_bss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _e_bss = .;
    } >RAM
}
