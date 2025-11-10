    .syntax unified
    .thumb

    .global Reset_Handler
    .global default_handler

    /* The vector table. The linker script will place this at the start of FLASH. */
    .section .isr_vector, "a"
    .align 2
    .word _estack            /* Initial Stack Pointer */
    .word Reset_Handler      /* Reset Handler */
    .word default_handler    /* NMI Handler */
    .word default_handler    /* HardFault Handler */
    .word default_handler    /* MemManage Handler */
    .word default_handler    /* BusFault Handler */
    .word default_handler    /* UsageFault Handler */
    .word 0                  /* Reserved */
    .word 0                  /* Reserved */
    .word 0                  /* Reserved */
    .word 0                  /* Reserved */
    .word SVC_Handler        /* SVCall Handler */
    .word default_handler    /* Debug Monitor Handler */
    .word 0                  /* Reserved */
    .word PendSV_Handler     /* PendSV Handler */
    .word SysTick_Handler    /* SysTick Handler */

    .size .isr_vector, . - .isr_vector

    .text
    .type Reset_Handler, %function
Reset_Handler:
    /* Set stack pointer */
    ldr r0, =_estack
    mov sp, r0

    /*
     * For a real project, you would copy .data from FLASH to RAM
     * and zero out the .bss section here. For this simple QEMU
     * example, we can skip it.
     */

    /* Call main */
    bl main

    /* If main returns, loop forever */
    b .

    .size Reset_Handler, . - Reset_Handler

    .type default_handler, %function
default_handler:
    b .

    .end
