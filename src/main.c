#include "../include/rtos.h"

extern void uart_init(void);
extern void uart_puts(const char *str);

rtos_mutex_t uart_mutex;

void task1_handler(void)
{
	while (1) {
		rtos_mutex_acquire(&uart_mutex);
		uart_puts("Task 1: Holding the mutex!\r\n");
		rtos_delay(1000); // Hold mutex for 1000 ticks (1 second)
		uart_puts("Task 1: Releasing the mutex!\r\n");
		rtos_mutex_release(&uart_mutex);
		rtos_delay(500); // Wait before trying again
	}
}

void task2_handler(void)
{
	while (1) {
		rtos_mutex_acquire(&uart_mutex);
		uart_puts("Task 2: Got the mutex now!\r\n");
		rtos_delay(800); // Hold for 800ms
		rtos_mutex_release(&uart_mutex);
		rtos_delay(300);
	}
}

int main(void)
{
	// Initialize hardware (clocks, UART, etc.)
	uart_init();

	// Initialize the RTOS
	rtos_init();
	rtos_mutex_init(&uart_mutex);

	// Create the tasks
	rtos_create_task(task1_handler);
	rtos_create_task(task2_handler);

	// Start the RTOS scheduler
	// This function will not return.
	rtos_start();

	return 0; // Should never be reached
}
