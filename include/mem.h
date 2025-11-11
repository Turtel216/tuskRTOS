/**
 * @file mem.h
 * @brief Interface for Tusk RTOS memory allocator.
 * @author Dimitrios Papakonstantinou
 *
 * A fixed-block memory pool for deterministic allocation times (O(1))
*/

#ifndef MEM_H_
#define MEM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "sync.h"

/**
 * @brief Memory Pool Control Block
 *        Holds all metadata for a memory pool instance.
 */
typedef struct {
	uint8_t *pool_start; // Pointer to the beginning of the underlying memory buffer
	void **next_free_block; // Pointer to the head of the free-list
	size_t num_blocks; // Total number of blocks in the pool
	size_t block_size; // Size of each individual block in bytes
	size_t used_blocks; // Number of blocks currently allocated (for stats)
	tusk_mutex_t mutex; // Mutex for thread-safe access
} mem_pool_t;

/**
 * @brief Initializes a fixed-block memory pool.
 *
 * This function sets up the internal free list and prepares the pool for use.
 * It must be called once before any other pool operations.
 *
 * @param pool Pointer to the memory pool structure to initialize.
 * @param pool_buffer Pointer to a contiguous block of memory to be used for the pool.
 * @param pool_size The total size of the pool_buffer in bytes.
 * @param block_size The desired size for each individual memory block.
 *
 * @return true if initialization was successful, false on failure (e.g., pool_size is too small).
 */
bool mem_pool_init(mem_pool_t *pool, void *pool_buffer, size_t pool_size,
		   size_t block_size);

/**
 * @brief Allocates a memory block from the pool.
 *
 * This function is thread-safe. It will block if the associated mutex is currently held.
 *
 * @param pool Pointer to the initialized memory pool.
 * @return A pointer to an allocated memory block, or NULL if no blocks are available.
 */
void *mem_pool_alloc(mem_pool_t *pool);

/**
 * @brief Frees a previously allocated memory block, returning it to the pool.
 *
 * This function is thread-safe.
 *
 * @param pool Pointer to the initialized memory pool.
 * @param block Pointer to the memory block to be freed. The block must have been
 *              previously allocated from this same pool. Behavior is undefined if
 *              a foreign pointer is passed.
 */
void mem_pool_free(mem_pool_t *pool, void *block);

/**
 * @brief Deinitializes a memory pool and destroys the associated mutex.
 *
 * @param pool Pointer to the memory pool to deinitialize.
 */
void mem_pool_deinit(mem_pool_t *pool);

/**
 * @brief Gets the number of used blocks in the pool.
 *
 * @param pool Pointer to the memory pool.
 * @return The number of currently allocated blocks.
 */
size_t mem_pool_get_used_count(mem_pool_t *pool);

#endif // MEM_H_
