#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>
#include "core_cm4.h"

#define MAX_TASKS 5
#define STACK_SIZE 1024 // 1KB stack per task

// Task states
#define TASK_RUNNING 0
#define TASK_READY 1
#define TASK_BLOCKED 2

// Mutex states
#define MUTEX_UNLOCKED 1
#define MUTEX_LOCKED 0

// Forward declaration
struct tcb;

/* Mutex structure */
typedef struct {
	volatile uint8_t locked;
	struct tcb *owner;
	struct tcb *waiting_list; // Simple linked list of waiting tasks
} rtos_mutex_t;

/* Semaphore structure */
typedef struct {
	volatile int32_t count;
	struct tcb *waiting_list; // Simple linked list of waiting tasks
} rtos_semaphore_t;

/* Task Control Block (TCB) */
typedef struct tcb {
	uint32_t *stack_pointer; // Must be the first element
	uint8_t state;
	uint32_t wakeup_time;
	struct tcb *next_tcb;
	struct tcb *wait_next; // For waiting lists on mutex/semaphore
} tcb_t;

/* Public Functions */
void rtos_init(void);
int rtos_create_task(void (*task_handler)(void));
void rtos_start(void);
void rtos_delay(uint32_t ticks);

void rtos_mutex_init(rtos_mutex_t *mutex);
void rtos_mutex_acquire(rtos_mutex_t *mutex);
void rtos_mutex_release(rtos_mutex_t *mutex);

void rtos_semaphore_init(rtos_semaphore_t *semaphore, int32_t initial_count);
void rtos_semaphore_wait(rtos_semaphore_t *semaphore);
void rtos_semaphore_post(rtos_semaphore_t *semaphore);

#endif // RTOS_H
