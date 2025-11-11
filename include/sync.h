#ifndef SYNC_H_
#define SYNC_H_

#include <stdint.h>

// Mutex states
#define MUTEX_UNLOCKED 1
#define MUTEX_LOCKED 0

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

void tusk_mutex_init(tusk_mutex_t *mutex);
void tusk_mutex_acquire(tusk_mutex_t *mutex);
void tusk_mutex_release(tusk_mutex_t *mutex);

void tusk_semaphore_init(rtos_semaphore_t *semaphore, int32_t initial_count);
void tusk_semaphore_wait(rtos_semaphore_t *semaphore);
void tusk_semaphore_post(rtos_semaphore_t *semaphore);

#endif // SYNC_H_
