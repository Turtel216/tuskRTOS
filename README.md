# tuskRTOS

## Overview
TuskRTOS is a lightweight, pre-emptive Real-Time Operating System (RTOS) designed specifically for ARM Cortex-M microcontrollers. It offers efficient task management, minimal memory overhead, and a straightforward API for real-time embedded applications. TuskRTOS is intended as an educational project and is not meant for use in production.

> [!NOTE]
> This project is still under active development and is not yet ready for to be used.

### Features
-[ ] Pre-emptive multitasking
-[x] Support for ARM Cortex-M architectures
-[ ] Configurable task priorities and time slices
<!-- - Inter-task communication via message queues and semaphores -->
-[ ] Dynamic memory management
-[ ] Static memory management

## Getting Started

### Prerequisites
To build and run tuskRTOS, you will need:
- ARM Cortex-M microcontroller (e.g., STM32, nRF52, or similar)
- GCC Arm Embedded Toolchain
- CMake (optional)

### Installation
Clone the repository:
```
git clone https://github.com/Turtel216/tuskRTOS.git
cd tuskRTOS
```

### Building the Project
Use the following command to build:
```
mkdir build && cd build
cmake ..
make
```

## Usage
Include the tuskRTOS header file in your application:
```c
#include "tuskRTOS.h"
```

Create and initialize tasks:
```c
tusk_create_task(myTaskFunction, priority);
tusk_start_scheduler();
```

### Example
```c
#include "tuskRTOS.h"

void task1(void) {
    while (1) {
        // Task code
    }
}

void task2(void) {
    while (1) {
        // Another task
    }
}

int main(void) {
    tusk_create_task(task1, 1);
    tusk_create_task(task2, 2);
    tusk_start_scheduler();
    return 0;
}
```

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request. For major changes, please open an issue first to discuss what you would like to change.

## License
TuskRTOS is licensed under the MIT License. See `LICENSE` for more information.
