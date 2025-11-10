#ifndef M_QUEUE_H_
#define M_QUEUE_H_

#include <stdint.h>
#include <stddef.h>
#include "core_cm4.h"

// --- Configuration ---
// Define the maximum number of messages the queue can hold.
#define QUEUE_MAX_MESSAGES 16

// Define the message data type.
// Using a void pointer allows for flexible message content.
typedef void *message_t;

// --- Queue Structure ---
typedef struct {
	message_t buffer[QUEUE_MAX_MESSAGES];
	volatile uint32_t head;
	volatile uint32_t tail;
	volatile uint32_t count;
} message_queue_t;

// --- Function Prototypes ---

/**
 * @brief Initializes a message queue.
 * @param q Pointer to the message_queue_t structure.
 */
void queue_init(message_queue_t *q);

/**
 * @brief Sends a message to the queue.
 * @param q Pointer to the message_queue_t structure.
 * @param message The message to send.
 * @return 0 on success, -1 if the queue is full.
 */
int32_t queue_send(message_queue_t *q, message_t message);

/**
 * @brief Receives a message from the queue.
 * @param q Pointer to the message_queue_t structure.
 * @param message Pointer to a message_t variable to store the received message.
 * @return 0 on success, -1 if the queue is empty.
 */
int32_t queue_receive(message_queue_t *q, message_t *message);

#endif // MESSAGE_QUEUE_H
