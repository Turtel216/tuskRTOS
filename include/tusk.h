/**
 * @file tusk.h
 * @brief Public interface for the Tusk Real-Time Operating System (RTOS) scheduler.
 * @author Dimitrios Papakonstantinou 
 *
 * This file defines the public API for interacting with the Tusk RTOS scheduler.
 * It includes definitions for task management, constants for task states, and the
 * structure for the Task Control Block (TCB).
 */

#ifndef TUSK_H
#define TUSK_H

#include <stdint.h>
#include "core_cm4.h"

/**
 * @def MAX_TASKS
 * @brief The maximum number of tasks that can be managed by the scheduler.
 */
#define MAX_TASKS 5

/**
 * @def STACK_SIZE
 * @brief The size of the stack for each task, in bytes.
 */
#define STACK_SIZE 1024 // 1KB stack per task

/**
 * @name Task States
 * @{
 */
/** @def TASK_RUNNING
 *  @brief A task state indicating that the task is currently executing. */
#define TASK_RUNNING 0

/** @def TASK_READY
 *  @brief A task state indicating that the task is ready to run but not currently executing. */
#define TASK_READY 1

/** @def TASK_BLOCKED
 *  @brief A task state indicating that the task is blocked, waiting for an event (e.g., a delay to expire). */
#define TASK_BLOCKED 2
/** @} */

// Forward declaration for the tcb struct.
struct tcb;

/**
 * @struct tcb
 * @brief Task Control Block (TCB).
 *
 * This structure holds all the necessary information for managing a task within the RTOS.
 * It includes the task's stack pointer, its current state, and pointers for scheduling.
 */
typedef struct tcb {
	/**
     * @var stack_pointer
     * @brief Pointer to the top of the task's stack.
     * @warning This must be the first element in the struct to ensure correct context switching.
     */
	uint32_t *stack_pointer;

	/**
     * @var state
     * @brief The current state of the task (e.g., TASK_RUNNING, TASK_READY, TASK_BLOCKED).
     */
	uint8_t state;

	/**
     * @var wakeup_time
     * @brief The system tick count at which a blocked task should be woken up.
     * This is used for tasks that are delayed via tusk_delay().
     */
	uint32_t wakeup_time;

	/**
     * @var next_tcb
     * @brief Pointer to the next TCB in the scheduler's list of tasks.
     * This is used to form a circular linked list for round-robin scheduling.
     */
	struct tcb *next_tcb;

	/**
     * @var wait_next
     * @brief Pointer to the next TCB in a waiting list for a resource like a mutex or semaphore.
     */
	struct tcb *wait_next;
} tcb_t;

/* Public Functions */

/**
 * @brief Initializes the Tusk RTOS scheduler.
 *
 * This function must be called before any other Tusk RTOS function. It sets up
 * the necessary data structures and timers for the scheduler to operate.
 */
void tusk_init(void);

/**
 * @brief Creates a new task and adds it to the scheduler.
 *
 * @param task_handler A pointer to the function that implements the task's behavior.
 *                     This function should have a `void (*)(void)` signature and should
 *                     not return.
 * @return int 0 on success, or a negative value on failure (e.g., if MAX_TASKS is exceeded).
 */
int tusk_create_task(void (*task_handler)(void));

/**
 * @brief Starts the Tusk RTOS scheduler and begins multitasking.
 *
 * This function starts the scheduler and initiates the context switching process.
 * It will start executing the first task created. This function does not return.
 */
void tusk_start(void);

/**
 * @brief Delays the calling task for a specified number of system ticks.
 *
 * The calling task is placed in the TASK_BLOCKED state, and the scheduler
 * will run other ready tasks. The task will be moved back to the TASK_READY
 * state after the specified number of ticks has elapsed.
 *
 * @param ticks The number of system ticks to delay the task.
 */
void tusk_delay(uint32_t ticks);

#endif // TUSK_H
