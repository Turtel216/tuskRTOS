/*/**
 * @file m_queue.h
 * @brief Public interface for a simple message queue for inter-task communication.
 * @author Dimitrios Papakonstantinou
 * @date 2025-11-11
 *
 * This file defines a basic FIFO (First-In, First-Out) message queue.
 * It is designed for passing messages between different tasks within the RTOS.
 * The queue stores pointers to messages, allowing for flexible data transfer.
 * This implementation is thread-safe for single-core processors, as critical
 * sections of the queue operations are protected by disabling interrupts
 * to ensure atomic access.
 */

#ifndef M_QUEUE_H_
#define M_QUEUE_H_

#include <stdint.h>
#include <stddef.h>
#include "core_cm4.h"

// --- Configuration ---

/**
 * @def QUEUE_MAX_MESSAGES
 * @brief The maximum number of messages that the queue can hold.
 */
#define QUEUE_MAX_MESSAGES 16

/**
 * @typedef message_t
 * @brief Defines the data type for a message.
 *
 * A `void` pointer is used to allow messages of any type to be sent.
 * The sender and receiver must agree on the actual data type being passed.
 * The pointer itself is what is stored in the queue.
 */
typedef void *message_t;

// --- Queue Structure ---

/**
 * @struct message_queue_t
 * @brief A FIFO message queue structure.
 *
 * This structure holds the state of a message queue, including the buffer
 * for storing message pointers and indices for managing the queue.
 */
typedef struct {
	/**
     * @var buffer
     * @brief The underlying circular buffer that stores the message pointers.
     */
	message_t buffer[QUEUE_MAX_MESSAGES];

	/**
     * @var head
     * @brief The index where the next message will be added.
     */
	volatile uint32_t head;

	/**
     * @var tail
     * @brief The index from which the next message will be retrieved.
     */
	volatile uint32_t tail;

	/**
     * @var count
     * @brief The current number of messages in the queue.
     */
	volatile uint32_t count;
} message_queue_t;

// --- Function Prototypes ---

/**
 * @brief Initializes a message queue.
 *
 * This function must be called before using the message queue. It sets the
 * head, tail, and count to their initial empty-state values.
 *
 * @param q Pointer to the `message_queue_t` structure to initialize.
 */
void queue_init(message_queue_t *q);

/**
 * @brief Sends a message to the back of the queue.
 *
 * This function adds a message pointer to the queue. This is a non-blocking call.
 * For thread safety on single-core processors, this function should be
 * called from within a critical section (e.g., with interrupts disabled).
 *
 * @param q Pointer to the `message_queue_t` structure.
 * @param message The message (a `void` pointer) to send.
 * @return `0` on success.
 * @return `-1` if the queue is full and the message could not be sent.
 */
int32_t queue_send(message_queue_t *q, message_t message);

/**
 * @brief Receives a message from the front of the queue.
 *
 * This function retrieves a message pointer from the queue. This is a non-blocking call.
 * For thread safety on single-core processors, this function should be
 * called from within a critical section (e.g., with interrupts disabled).
 *
 * @param q Pointer to the `message_queue_t` structure.
 * @param message Pointer to a `message_t` variable where the received message pointer will be stored.
 * @return `0` on success.
 * @return `-1` if the queue is empty.
 */
int32_t queue_receive(message_queue_t *q, message_t *message);

#endif // M_QUEUE_H_
