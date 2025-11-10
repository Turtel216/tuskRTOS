#include "../include/kernel.h"
#include <stddef.h> // For NULL

// --- Kernel Globals ---
tcb_t tasks[MAX_TASKS];
uint32_t task_stacks[MAX_TASKS][STACK_SIZE];
tcb_t *current_tcb = NULL;
uint32_t task_count = 0;
volatile uint32_t rtos_ticks = 0;

// --- Private Function Prototypes ---
void rtos_scheduler(void);
void add_to_wait_list(struct tcb **list, tcb_t *task);
tcb_t *remove_from_wait_list(struct tcb **list);

/*
 * SVC_Handler and PendSV_Handler are defined in rtos_asm.s
 * These are the entry points for starting the first task and context switching.
 */
extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void rtos_start(void); // Assembly function to trigger SVC

/* SysTick_Handler - The heart of the preemptive scheduler */
void SysTick_Handler(void)
{
	rtos_ticks++;

	// Check for any sleeping tasks that need to wake up
	for (uint32_t i = 0; i < task_count; i++) {
		if (tasks[i].state == TASK_BLOCKED &&
		    tasks[i].wakeup_time > 0 &&
		    rtos_ticks >= tasks[i].wakeup_time) {
			tasks[i].state = TASK_READY;
			tasks[i].wakeup_time = 0;
		}
	}

	// Trigger PendSV to run the scheduler and perform a context switch
	// SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	// Using a direct address for compatibility
	*(volatile uint32_t *)0xE000ED04 |= (1 << 28);
}

void rtos_init(void)
{
	// Reset all tasks
	for (int i = 0; i < MAX_TASKS; i++) {
		tasks[i].state = 0; // Inactive
		tasks[i].stack_pointer = NULL;
	}
	current_tcb = &tasks[0]; // Start with the first task

	// Configure SysTick for a 1ms tick (assuming a 16MHz clock for simplicity)
	// You MUST adjust this value for your actual system clock frequency.
	// Example: For 16MHz clock, to get 1000Hz (1ms tick): 16,000,000 / 1000 = 16,000
	// SysTick->LOAD = 16000 - 1;
	// SysTick->VAL = 0;
	// SysTick->CTRL = 0x07; // Enable, Use Processor Clock, Enable Interrupt
}

int rtos_create_task(void (*task_handler)(void))
{
	if (task_count >= MAX_TASKS) {
		return -1; // Error: Max tasks reached
	}

	tcb_t *new_tcb = &tasks[task_count];
	uint32_t *stack_top = &task_stacks[task_count][STACK_SIZE - 1];

	// Initialize the task stack for Cortex-M
	*(--stack_top) = 0x01000000; // xPSR (Thumb state)
	*(--stack_top) = (uint32_t)task_handler; // PC (program counter)
	*(--stack_top) = 0; // LR (link register)
	*(--stack_top) = 0; // R12
	*(--stack_top) = 0; // R3
	*(--stack_top) = 0; // R2
	*(--stack_top) = 0; // R1
	*(--stack_top) = 0; // R0
	// The processor automatically saves R4-R11
	*(--stack_top) = 0; // R11
	*(--stack_top) = 0; // R10
	*(--stack_top) = 0; // R9
	*(--stack_top) = 0; // R8
	*(--stack_top) = 0; // R7
	*(--stack_top) = 0; // R6
	*(--stack_top) = 0; // R5
	*(--stack_top) = 0; // R4

	new_tcb->stack_pointer = stack_top;
	new_tcb->state = TASK_READY;
	new_tcb->next_tcb = &tasks[(task_count + 1) % task_count];
	if (task_count > 0) {
		tasks[task_count - 1].next_tcb = new_tcb;
	}

	task_count++;
	return 0; // Success
}

/* Scheduler Logic (Simple Round-Robin) */
void rtos_scheduler(void)
{
	tcb_t *next_task = current_tcb;
	do {
		next_task = next_task->next_tcb;
	} while (next_task->state != TASK_READY && next_task != current_tcb);

	if (next_task->state == TASK_READY) {
		current_tcb = next_task;
	}
	// If no other task is ready, we just continue with the current one.
}

/* --- Synchronization Primitives --- */

void rtos_delay(uint32_t ticks)
{
	if (ticks == 0)
		return;
	current_tcb->wakeup_time = rtos_ticks + ticks;
	current_tcb->state = TASK_BLOCKED;
	// Trigger scheduler to switch to another task
	*(volatile uint32_t *)0xE000ED04 |= (1 << 28); // Set PendSV
}

void rtos_mutex_init(rtos_mutex_t *mutex)
{
	mutex->locked = MUTEX_UNLOCKED;
	mutex->owner = NULL;
	mutex->waiting_list = NULL;
}

void rtos_mutex_acquire(rtos_mutex_t *mutex)
{
	__disable_irq(); // Enter critical section
	if (mutex->locked == MUTEX_LOCKED) {
		// Mutex is taken, block the current task
		current_tcb->state = TASK_BLOCKED;
		add_to_wait_list(&mutex->waiting_list, current_tcb);
		__enable_irq(); // Re-enable interrupts BEFORE scheduling
		*(volatile uint32_t *)0xE000ED04 |=
			(1 << 28); // Trigger scheduler
	} else {
		// Mutex is free, take it
		mutex->locked = MUTEX_LOCKED;
		mutex->owner = current_tcb;
		__enable_irq(); // Exit critical section
	}
}

void rtos_mutex_release(rtos_mutex_t *mutex)
{
	__disable_irq(); // Enter critical section
	if (mutex->owner == current_tcb) {
		tcb_t *unblocked_task =
			remove_from_wait_list(&mutex->waiting_list);
		if (unblocked_task != NULL) {
			// Give the mutex to the next waiting task
			mutex->owner = unblocked_task;
			unblocked_task->state = TASK_READY;
		} else {
			// No tasks waiting, just unlock
			mutex->locked = MUTEX_UNLOCKED;
			mutex->owner = NULL;
		}
	}
	__enable_irq(); // Exit critical section
}

void rtos_semaphore_init(rtos_semaphore_t *semaphore, int32_t initial_count)
{
	semaphore->count = initial_count;
	semaphore->waiting_list = NULL;
}

void rtos_semaphore_wait(rtos_semaphore_t *semaphore)
{
	__disable_irq();
	semaphore->count--;
	if (semaphore->count < 0) {
		// Resource not available, block the task
		current_tcb->state = TASK_BLOCKED;
		add_to_wait_list(&semaphore->waiting_list, current_tcb);
		__enable_irq();
		*(volatile uint32_t *)0xE000ED04 |=
			(1 << 28); // Trigger scheduler
	} else {
		__enable_irq();
	}
}

void rtos_semaphore_post(rtos_semaphore_t *semaphore)
{
	__disable_irq();
	semaphore->count++;
	if (semaphore->count <= 0) {
		// Tasks are waiting, unblock one
		tcb_t *unblocked_task =
			remove_from_wait_list(&semaphore->waiting_list);
		if (unblocked_task != NULL) {
			unblocked_task->state = TASK_READY;
		}
	}
	__enable_irq();
}

/* --- Helper functions for managing wait lists --- */

void add_to_wait_list(struct tcb **list, tcb_t *task)
{
	task->wait_next = NULL;
	if (*list == NULL) {
		*list = task;
	} else {
		tcb_t *temp = *list;
		while (temp->wait_next != NULL) {
			temp = temp->wait_next;
		}
		temp->wait_next = task;
	}
}

tcb_t *remove_from_wait_list(struct tcb **list)
{
	if (*list == NULL) {
		return NULL;
	}
	tcb_t *task = *list;
	*list = task->wait_next;
	task->wait_next = NULL;
	return task;
}
