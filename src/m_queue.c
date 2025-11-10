#include "../include/m_queue.h"

void queue_init(message_queue_t *q)
{
	q->head = 0;
	q->tail = 0;
	q->count = 0;
}

int32_t queue_send(message_queue_t *q, message_t message)
{
	__disable_irq();

	if (q->count >= QUEUE_MAX_MESSAGES) { // Queue is full
		__enable_irq();
		return -1;
	}

	q->buffer[q->tail] = message;
	q->tail = (q->tail + 1) % QUEUE_MAX_MESSAGES;
	q->count++;

	__enable_irq();
	return 0;
}

int32_t queue_receive(message_queue_t *q, message_t *message)
{
	__disable_irq();

	if (q->count == 0) { // Queue is empty
		__enable_irq();
		return -1;
	}

	*message = q->buffer[q->head];
	q->head = (q->head + 1) % QUEUE_MAX_MESSAGES;
	q->count--;

	__enable_irq();
	return 0;
}
