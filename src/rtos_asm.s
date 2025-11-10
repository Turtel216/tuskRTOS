    .syntax unified
    .thumb
    .global SVC_Handler
    .global PendSV_Handler
    .global rtos_start

    .equ SCB_VTOR, 0xE000ED08

    .type SVC_Handler, %function
SVC_Handler:
    // This handler is called to start the first task.
    // It loads the address of current_tcb into R0
    ldr r0, =current_tcb
    ldr r1, [r0] // R1 = value of current_tcb (address of the first TCB)
    ldr sp, [r1] // Set the main stack pointer to the task's stack pointer

    // Pop registers R4-R11 from the task's stack
    pop {r4-r11}
    // Pop registers R0-R3, R12, LR, PC, xPSR
    pop {r0-r3, r12, lr}
    pop {r2} // Dummy pop to align stack
    mov lr, r2 // Put LR back
    cpsie i // Enable interrupts
    bx lr   // Branch to the task code

    .type PendSV_Handler, %function
PendSV_Handler:
    // --- Context Switch ---
    // This is the core of the preemptive context switch.

    // 1. Disable interrupts to prevent race conditions
    cpsid i

    // 2. Save the context of the current task
    // Processor already pushed R0-R3, R12, LR, PC, xPSR automatically.
    // We need to save the rest: R4-R11.
    mrs r0, psp // Get the current process stack pointer
    stmdb r0!, {r4-r11} // Store R4-R11 on the task's stack (and decrement SP)

    // 3. Save the updated stack pointer back to the TCB
    ldr r1, =current_tcb
    ldr r2, [r1] // R2 = address of current TCB
    str r0, [r2] // Save the new stack pointer value into the TCB

    // 4. Call the C scheduler to choose the next task
    bl rtos_scheduler

    // 5. Restore the context of the next task
    // current_tcb has now been updated by rtos_scheduler
    ldr r1, =current_tcb
    ldr r2, [r1] // R2 = address of the NEW current TCB
    ldr r0, [r2] // R0 = stack pointer of the new task

    // 6. Load the new task's R4-R11 from its stack
    ldmia r0!, {r4-r11}

    // 7. Update the process stack pointer
    msr psp, r0

    // 8. Enable interrupts
    cpsie i

    // 9. Return from exception. The processor will automatically
    //    unstack R0-R3, R12, LR, PC, xPSR and resume the new task.
    mov r0, #0xFFFFFFF9 // Use Thread mode, PSP stack
    mov lr, #0xFFFFFFFD // Special value to return to thread mode using PSP
    bx lr

    .type rtos_start, %function
rtos_start:
    // Set PendSV and SVC to the lowest priority
    ldr r0, =0xE000ED20
    ldr r1, [r0]
    orr r1, #0xF0F00000 // Set priority for PendSV and SVCall
    str r1, [r0]

    // Start the first task by triggering the SVC exception
    cpsie i
    svc 0
    // We should never return here
    nop

