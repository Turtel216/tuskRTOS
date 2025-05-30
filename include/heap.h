/**
 * @file heap.h
 *
 * @brief Implements a dynamic memory allocator
 *
 * This module provides an interface for thread safe
 * dynamic memory managment.
 *
 * @note The default heap size is 4096
*/

#ifndef HEAP_H_
#define HEAP_H_

#include <stddef.h>

/**
 * @brief Initializes the heap memory as a single free block.
 *
 * This function sets up the initial heap structure by configuring the first block
 * to span the entire available heap memory. It must be called before any calls to
 * `kmalloc`, `kfree`, etc.
 *
 */
void heap_init(void);

/**
 * @brief Allocates a block of memory of the given size.
 *
 * Searches the free list for the first suitable block (first-fit algorithm), splits it if
 * there's remaining space, and returns a pointer to the allocated memory.
 *
 * @param size Requested allocation size in bytes.
 * @return void* Pointer to the allocated memory, or `NULL` if allocation fails.
 *
 * @note The allocated memory is not initialized (like standard `malloc`).
 * @note Size is aligned to the nearest `sizeof(size_t)` boundary.
 */
void *kmalloc(size_t size);

/**
 * @brief Releases a previously allocated block of memory.
 *
 * Marks the block as free and merges it with adjacent free blocks to reduce fragmentation.
 *
 * @param ptr Pointer to the memory block to free (must be a pointer returned by `malloc`).
 *
 * @warning Passing an invalid pointer (not allocated by `malloc`) causes undefined behavior.
 */
void kfree(void *ptr);

/**
 * @brief Allocates memory for an array and initializes it to zero.
 *
 * @param num Number of elements.
 * @param size Size of each element.
 * @return void* Pointer to the allocated memory, or `NULL` if allocation fails.
 *
 * @note Uses `kmalloc` internally, followed by zero-filling.
 */
void *kcalloc(size_t num, size_t size);

/**
 * @brief Resizes a previously allocated memory block.
 *
 * If the new size is larger, the contents of the old block are copied to the new block.
 * If the new size is smaller or equal, the original block may be retained.
 *
 * @param ptr Pointer to the previously allocated memory (or `NULL` for new allocation).
 * @param new_size New size in bytes.
 * @return void* Pointer to the resized memory, or `NULL` if allocation fails.
 *
 * @note If `ptr` is `NULL`, behaves like `kmalloc(new_size)`.
 * @note If `new_size` is 0, behaves like `kfree(ptr)` and returns `NULL`.
 */
void *realloc(void *ptr, size_t new_size);

#endif // HEAP_H_
