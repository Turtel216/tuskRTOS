# tuskRTOS

## Overview
TuskRTOS is a lightweight, pre-emptive Real-Time Operating System (RTOS) designed specifically for ARM Cortex-M microcontrollers. It offers efficient task management, minimal memory overhead, and a straightforward API for real-time embedded applications. TuskRTOS is intended as an educational project and is not meant for use in production.

> [!NOTE]
> This project is still under active development and is not yet ready for to be used.

### Features
- [x] Support for ARM Cortex-M4 architectures
- [x] Pre-emptive scheduling.
- [ ] Priority scheduling
- [x] Syncronization via Mutexs
- [x] Inter-task communication via semaphores
- [ ] Inter-task communication via message queues 
- [ ] Fixed-Block Memory Pool allocator

## Getting Started

### Prerequisites
To build and run tuskRTOS, you will need:
- ARM Cortex-M microcontroller (e.g., STM32, nRF52, or similar)
- GCC Arm Embedded Toolchain

### Installation
Clone the repository:
```
git clone https://gitlab.com/papakonstantinou/tuskRTOS.git
cd tuskRTOS
```

### Building the Project
Use the following command to build:
```
make
```
To run the program in QEMU use

```
make run
```

And to clean up the build files afterwards run

```
make clean
```

## Usage
Include the **rtos** header file in your application:
```c
#include "include/rtos.h"
```

Create and initialize tasks:
```c
rtos_init(); // Startup the OS
rtos_create_task(myTaskFunction); // Create a new task
rtos_start(); // This function shouldn't return
```

### Example
```c
#include "../include/rtos.h"

// Assume you have a function to print strings, e.g., uart_puts()
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
```

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request. For major changes, please open an issue first to discuss what you would like to change.

## License
TuskRTOS is licensed under the MIT License. See `LICENSE` for more information.
