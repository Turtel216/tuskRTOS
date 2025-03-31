#include "../include/kernel.h"
#include "../include/scheduler.h"

// Start up all kernel systems
void init_kernel(void)
{
	// Initialize scheduler
	os_init_stack();
}
