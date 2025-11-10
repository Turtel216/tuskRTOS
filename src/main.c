#include "../include/tusk.h"
#include "../include/serial.h"

tusk_mutex_t uart_mutex;

void task1_handler(void)
{
	while (1) {
		tusk_mutex_acquire(&uart_mutex);
		serial_print("Task 1: Holding the mutex!\r\n");
		tusk_delay(1000); // Hold mutex for 1000 ticks (1 second)
		serial_print("Task 1: Releasing the mutex!\r\n");
		tusk_mutex_release(&uart_mutex);
		tusk_delay(500); // Wait before trying again
	}
}

void task2_handler(void)
{
	while (1) {
		tusk_mutex_acquire(&uart_mutex);
		serial_print("Task 2: Got the mutex now!\r\n");
		tusk_delay(800); // Hold for 800ms
		tusk_mutex_release(&uart_mutex);
		tusk_delay(300);
	}
}

int main(void)
{
	// Initialize hardware (clocks, UART, etc.)
	uart_init();

	// Initialize the RTOS
	tusk_init();
	tusk_mutex_init(&uart_mutex);

	// Create the tasks
	tusk_create_task(task1_handler);
	tusk_create_task(task2_handler);

	// Start the RTOS scheduler
	// This function will not return.
	tusk_start();

	return 0; // Should never be reached
}
