/**
 * @file scheduler.h
 *
 * @brief Implements a Pre-Emptive Scheduler for ARM Cortex-M architectures
*/

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/**
 * @brief Initializes the task stack
 *
 * @note The stack contains a task contedt even during the first context switch
*/
void os_init_stack(void);

#endif // SCHEDULER_H_
