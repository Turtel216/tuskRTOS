/**
 * @file sync.h
 * @brief Public interface for Tusk RTOS synchronization primitives.
 * @author Dimitrios Papakonstantinou
 *
 * This file defines the public API for mutexes and semaphores in the Tusk RTOS.
 * These primitives are essential for managing access to shared resources and for
 * inter-task communication and synchronization.
 */

#ifndef SYNC_H_
#define SYNC_H_

#include <stdint.h>
#include "tusk.h" // Include for tcb struct definition

/**
 * @name Mutex States
 * @{
 */
/** @def MUTEX_UNLOCKED
 *  @brief A mutex state indicating that the mutex is not currently held and can be acquired. */
#define MUTEX_UNLOCKED 1

/** @def MUTEX_LOCKED
 *  @brief A mutex state indicating that the mutex is currently held by a task. */
#define MUTEX_LOCKED 0
/** @} */

/**
 * @struct tusk_mutex_t
 * @brief A mutual exclusion (mutex) primitive.
 *
 * This structure provides a mechanism to ensure that only one task can access a
 * shared resource at a time. Tasks waiting to acquire a locked mutex are
- * placed in a waiting list and blocked until the mutex is released.
 */
typedef struct {
	/**
     * @var locked
     * @brief The current state of the mutex (MUTEX_LOCKED or MUTEX_UNLOCKED).
     */
	volatile uint8_t locked;

	/**
     * @var owner
     * @brief A pointer to the Task Control Block (TCB) of the task that currently holds the mutex.
     * Is NULL if the mutex is unlocked.
     */
	struct tcb *owner;

	/**
     * @var waiting_list
     * @brief A pointer to the head of a linked list of tasks that are blocked waiting to acquire this mutex.
     */
	struct tcb *waiting_list;
} tusk_mutex_t;

/**
 * @struct rtos_semaphore_t
 * @brief A counting semaphore primitive.
 *
 * This structure provides a classic counting semaphore mechanism. Semaphores can be used
 * to control access to a resource with multiple instances or for signaling between tasks.
 */
typedef struct {
	/**
     * @var count
     * @brief The current count of the semaphore. If the count is positive, a task can
     * take the semaphore without blocking. If it is zero or negative, the task will block.
     */
	volatile int32_t count;

	/**
     * @var waiting_list
     * @brief A pointer to the head of a linked list of tasks that are blocked waiting for this semaphore.
     */
	struct tcb *waiting_list;
} rtos_semaphore_t;

/**
 * @brief Initializes a mutex.
 *
 * This function sets the mutex to its initial unlocked state, ready for use.
 *
 * @param mutex A pointer to the `tusk_mutex_t` object to be initialized.
 */
void tusk_mutex_init(tusk_mutex_t *mutex);

/**
 * @brief Acquires a mutex.
 *
 * If the mutex is currently unlocked, the calling task acquires it and continues execution.
 * If the mutex is locked by another task, the calling task is blocked and placed on
 * the mutex's waiting list until the mutex is released.
 *
 * @param mutex A pointer to the `tusk_mutex_t` object to be acquired.
 */
void tusk_mutex_acquire(tusk_mutex_t *mutex);

/**
 * @brief Releases a mutex.
 *
 * The mutex is unlocked. If there are tasks waiting to acquire the mutex,
 * the highest-priority waiting task is unblocked and given the mutex.
 *
 * @param mutex A pointer to the `tusk_mutex_t` object to be released.
 *              The mutex must be owned by the calling task.
 */
void tusk_mutex_release(tusk_mutex_t *mutex);

/**
 * @brief Initializes a semaphore.
 *
 * Sets the semaphore to an initial count.
 *
 * @param semaphore A pointer to the `rtos_semaphore_t` object to be initialized.
 * @param initial_count The initial value for the semaphore's counter.
 */
void tusk_semaphore_init(rtos_semaphore_t *semaphore, int32_t initial_count);

/**
 * @brief Waits for (or decrements) a semaphore.
 *
 * Also known as "pend" or "P" operation. If the semaphore's count is greater than zero,
 * it is decremented, and the task continues. If the count is zero, the task is blocked
 * until another task posts to the semaphore.
 *
 * @param semaphore A pointer to the `rtos_semaphore_t` object.
 */
void tusk_semaphore_wait(rtos_semaphore_t *semaphore);

/**
 * @brief Posts to (or increments) a semaphore.
 *
 * Also known as "signal" or "V" operation. The semaphore's count is incremented.
 * If there are tasks blocked waiting for the semaphore, the highest-priority
 * waiting task is unblocked.
 *
 * @param semaphore A pointer to the `rtos_semaphore_t` object.
 */
void tusk_semaphore_post(rtos_semaphore_t *semaphore);

#endif // SYNC_H_
