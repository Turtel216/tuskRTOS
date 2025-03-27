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
int32_t tch_stack[NUM_OF_THREADS][STACKSIZE];
