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

// Forward declaration
struct tcb;

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

#endif // RTOS_H
