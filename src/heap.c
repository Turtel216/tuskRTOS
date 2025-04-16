#include "../include/heap.h"
#include <stdint.h>

// Size of heap memory region
#define HEAP_SIZE 4096
// Size of a heap memory region
#define BLOCK_SIZE sizeof(struct block)

/**
 * @brief Metadata structure for memory blocks in the heap.
 *
 * Each block of memory (allocated or free) is prefixed by this header, which tracks:
 * - Size of the user-accessible memory (excluding metadata).
 * - Pointer to the next block in the free list.
 * - Whether the block is free or allocated.
 */
struct block {
	size_t size;
	struct block *next;
	uint8_t free;
};

// Heap memory region
static struct block heap[HEAP_SIZE];
// Linked list of avaible memory blocks
static struct block *free_list = NULL;

/**
 * @brief Initializes the heap memory as a single free block.
 *
 * This function sets up the initial heap structure by configuring the first block
 * to span the entire available heap memory. It must be called before any calls to
 * `kmalloc`, `kfree`, etc.
 *
 */
void heap_init(void)
{
	// Create initial free block
	free_list->size = HEAP_SIZE - BLOCK_SIZE;
	free_list->free = 1;
	free_list->next = NULL;
}
