/**
 * @file sync.h
 *
 * @brief Provides interface for data access synchronization
 *
 * @note The lock mechanism is implemented using a spin lock. This implementation may reduce perfomance
*/

#ifndef SYNC_H_
#define SYNC_H_

#include <stdint.h>

/**
 * @brief Mutex type for syncronizing data access
 *
 * @note Implemented as uint32_t internally
*/
typedef uint32_t kmutex;

/**
 * @brief Acquire a mutex lock.
 *
 * This function spins until the lock is successfully acquired using LDREX/STREX.
 *
 * @param lock Pointer to the volatile mutex variable.
 */
void kmutex_lock(volatile kmutex *lock);

/**
 * @brief Release a mutex lock.
 *
 * This function writes zero to the lock variable, thus releasing the lock.
 *
 * @param lock Pointer to the volatile mutex variable.
 */
void kmutex_unlock(volatile kmutex *lock);

#endif // SYNC_H_
