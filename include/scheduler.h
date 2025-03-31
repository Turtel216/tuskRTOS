// Implements a Pre-Emptive Scheduler for the ARM Cortex-M

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

// Initializes the task stack. The stack contains a task context even
// during the first task context switch.
void os_init_stack(void);

#endif // SCHEDULER_H_
