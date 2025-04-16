#include "../include/sync.h"

/**
 * @brief Acquire a mutex lock.
 *
 * This function spins until the lock is successfully acquired using LDREX/STREX.
 *
 * @param lock Pointer to the volatile mutex variable.
 */
void kmutex_lock(volatile kmutex *lock)
{
	uint32_t result;
	uint32_t temp;
	do {
		__asm volatile(
			"ldrex %0, [%2]\n" // Load the value from *lock into temp.
			"cmp %0, #0\n" // Compare temp with 0.
			"strexeq %1, %3, [%2]\n" // If temp is 0, attempt to store '1' into *lock.
			: "=&r"(temp),
			  "=&r"(result) // Outputs: temp (the loaded value) and result (the status of store-exclusive)
			: "r"(lock),
			  "r"(1) // Inputs: pointer to the lock and value 1.
			: "cc",
			  "memory" // Clobbered: condition code and memory.
		);
	} while (result != 0);
}

/**
 * @brief Release a mutex lock.
 *
 * This function writes zero to the lock variable, thus releasing the lock.
 *
 * @param lock Pointer to the volatile mutex variable.
 */
void kmutex_unlock(volatile kmutex *lock)
{
	__asm volatile("str %1, [%0]" : : "r"(lock), "r"(0) : "memory");
}
