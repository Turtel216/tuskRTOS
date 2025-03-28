// Implements a Pre-Emptive Scheduler for the ARM Cortex-M

#include <stdint.h>

#define NUM_OF_THREADS 2
#define STACKSIZE 100

// A linked list implementation of the Task Control Block
struct task_control_block {
  int32_t *stack;                  // Pointer to the stack
  struct task_control_block *next; // Pointer to the control block of the task
};

// Array that stores control blocks for each task
struct task_control_block control_blocks[NUM_OF_THREADS];

// Pointer to the current active task
struct task_control_block *current_cb;

// Stack for each task
int32_t tcb_stack[NUM_OF_THREADS][STACKSIZE];

// Handles Systick interrupt and swaps tasks
__attribute__((naked)) void sys_tick_handler(void) {
  // Save the current task context

  // disable interrupts
  __asm("CPID    I");

  // Push registerds R4-R11
  __asm("PUSH    {R4-R7}");
  __asm("MOV    R4, R9");
  __asm("MOV    R5, R9");
  __asm("MOV    R6, R10");
  __asm("MOV    R7, R11");
  __asm("PUSH    {R4-R7}");

  // load R0 with the address of current control block
  __asm("LDR    R0, =pcurrent_cb");

  // load R1 with the content of the current control block
  __asm("LDR    R1, [R0]");

  // Move stack pointer to R4 and store its value to the current task
  __asm("MOV    R4, SP");
  __asm("STR    R4, [R1]");

  // Load the new task context to the CPU registers

  // Load the address of the next task onto R1
  __asm("LDR    R1, [R1, #4]");

  // Point currnt_cb to the newer task
  __asm("STR    R4, [R1]");

  // load newer task to SP
  __asm("LDR    R4, [R1]");
  __asm("MOV    SP, R4");

  // Pop registers R4-R11
  __asm("POP    {R4-R7}");
  __asm("MOV    R8, R4");
  __asm("MOV    R9, R5");
  __asm("MOV    R10, R6");
  __asm("MOV    R11, R7");
  __asm("POP    {R4-R7}");
  __asm("CPSIE  I ");
  __asm("BX     LR");
}

// Initializes the task stack. The stack contains a task context even
// during the first task context switch.
void os_init_stack(void) {
  // Disable interrupts
  __asm("CPSID    I");

  // Make the control group list circular
  control_blocks[0].next = &control_blocks[1];
  control_blocks[1].next = &control_blocks[0];

  // Setup stack for task0
  control_blocks[0].stack = &tcb_stack[0][STACKSIZE - 16];

  // Notify processor on exception return about the thumb state
  tcb_stack[0][STACKSIZE - 1] = 0x01000000;
  // Set stacked PC to the task
  tcb_stack[0][STACKSIZE - 2] = (int32_t)(Task0); // TODO

  // Setup stack for task1
  control_blocks[1].stack = &tcb_stack[1][STACKSIZE - 16];
  // Notify processor on exception return about the thumb state
  tcb_stack[1][STACKSIZE - 1] = 0x01000000;
  // Set the stacked PC to point to the task
  tcb_stack[1][STACKSIZE - 2](int32_t)(Task1); // TODO

  // Make current control block point to task9
  current_cb = &control_blocks[0];

  // Enable interrupts
  __asm("CPSIE    I");
}
