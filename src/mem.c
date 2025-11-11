#include "../include/mem.h"

// TODO: Replace commented asserts

// Ensuring block_size is a multiple of 4 good for alignment.
// We also require block_size to be at least the size of a pointer to form the free list.
#define MIN_BLOCK_SIZE_BYTES sizeof(void *)
#define ALIGNMENT_BYTES 4

/**
 * @brief Aligns a size up to the nearest multiple of ALIGNMENT_BYTES.
 */
static inline size_t align_up(size_t size)
{
	return (size + (ALIGNMENT_BYTES - 1)) & ~(ALIGNMENT_BYTES - 1);
}

bool mem_pool_init(mem_pool_t *pool, void *pool_buffer, size_t pool_size,
		   size_t block_size)
{
	// assert(pool != NULL);
	// assert(pool_buffer != NULL);

	if (pool == NULL || pool_buffer == NULL) {
		return false;
	}

	// 1. Ensure block size is valid. It must be large enough to hold a pointer
	//    for the free list and should be word-aligned for performance on Cortex-M.
	size_t actual_block_size = align_up(block_size);
	if (actual_block_size < MIN_BLOCK_SIZE_BYTES) {
		actual_block_size = MIN_BLOCK_SIZE_BYTES;
	}

	// 2. Calculate the total number of blocks that can fit in the buffer.
	size_t num_blocks = pool_size / actual_block_size;
	if (num_blocks == 0) {
		return false; // Pool buffer is too small to even hold one block.
	}

	// 3. Initialize the pool control block.
	pool->pool_start = (uint8_t *)pool_buffer;
	pool->num_blocks = num_blocks;
	pool->block_size = actual_block_size;
	pool->used_blocks = 0;
	tusk_mutex_init(&pool->mutex);

	// 4. Create the singly-linked list of free blocks.
	// The memory of the free blocks themselves is used to store the pointers.
	uint8_t *current_block = pool->pool_start;
	for (size_t i = 0; i < num_blocks - 1; ++i) {
		// Cast the current block to a pointer-to-pointer, and set it to point
		// to the next block in the sequence.
		*(void **)current_block = (current_block + actual_block_size);
		current_block += actual_block_size;
	}

	// 5. The last block in the list points to NULL, signifying the end.
	*(void **)current_block = NULL;

	// 6. The head of the free list is the first block in the pool.
	pool->next_free_block = (void **)pool->pool_start;

	return true;
}

void mem_pool_deinit(mem_pool_t *pool)
{
	if (pool == NULL) {
		return;
	}
	// For safety, you might want to assert that all blocks have been freed.
	// assert(pool->used_blocks == 0);
	tusk_mutex_release(&pool->mutex); // TODO deinit the mutex

	// Clear the structure to prevent accidental use-after-free
	pool->pool_start = NULL;
	pool->next_free_block = NULL;
	pool->num_blocks = 0;
	pool->block_size = 0;
	pool->used_blocks = 0;
	// pool->mutex = NULL;
}

void *mem_pool_alloc(mem_pool_t *pool)
{
	// assert(pool != NULL);
	if (pool == NULL) {
		return NULL;
	}

	tusk_mutex_acquire(&pool->mutex);

	// Check if there are any free blocks available.
	if (pool->next_free_block == NULL) {
		tusk_mutex_release(&pool->mutex);
		return NULL; // Pool is exhausted.
	}

	// Get the block from the head of the free list.
	void *allocated_block = pool->next_free_block;

	// Advance the free list head to the next available block.
	// The pointer to the next block is stored in the memory of the current free block.
	pool->next_free_block = *(void **)allocated_block;

	pool->used_blocks++;

	tusk_mutex_release(&pool->mutex);

	return allocated_block;
}

void mem_pool_free(mem_pool_t *pool, void *block)
{
	// assert(pool != NULL);
	// assert(block != NULL);

	if (pool == NULL || block == NULL) {
		return;
	}

	// Basic sanity check: ensure the block being freed is within the pool's memory range.
	// This is not foolproof but catches many common errors.
	// assert((uint8_t *)block >= pool->pool_start);
	// assert((uint8_t *)block < (pool->pool_start + (pool->num_blocks * pool->block_size)));

	tusk_mutex_acquire(&pool->mutex);

	// Add the freed block to the front of the free list.
	// The freed block's memory now stores the pointer to the old head.
	*(void **)block = pool->next_free_block;

	// The head of the free list is now the block we just freed.
	pool->next_free_block = (void **)block;

	// assert(pool->used_blocks > 0); // Cannot free more blocks than were allocated
	pool->used_blocks--;

	tusk_mutex_release(&pool->mutex);
}

size_t mem_pool_get_used_count(mem_pool_t *pool)
{
	// This is a quick, non-locking read. For a perfectly accurate number
	// in a high-contention system, a lock might be needed, but typically
	// this is sufficient for diagnostics.
	return (pool != NULL) ? pool->used_blocks : 0;
}
