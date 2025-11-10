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
} tusk_mutex_t;

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
void tusk_init(void);
int tusk_create_task(void (*task_handler)(void));
void tusk_start(void);
void tusk_delay(uint32_t ticks);

void tusk_mutex_init(tusk_mutex_t *mutex);
void tusk_mutex_acquire(tusk_mutex_t *mutex);
void tusk_mutex_release(tusk_mutex_t *mutex);

void tusk_semaphore_init(rtos_semaphore_t *semaphore, int32_t initial_count);
void tusk_semaphore_wait(rtos_semaphore_t *semaphore);
void tusk_semaphore_post(rtos_semaphore_t *semaphore);

#endif // RTOS_H
