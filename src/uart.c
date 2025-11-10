/*
 * A very simple UART driver for the QEMU STM32 machine (netduinoplus2).
 * It writes directly to the USART1 data register.
 */

// USART1 Data Register on the STM32F4 series
#define UART_DR (*(volatile unsigned int *)0x40011004)

void uart_init(void)
{
	// In a real application, we would configure clocks, pins, and baud rates.
	// For QEMU, this is not necessary. The serial port is already configured.
}

void uart_puts(const char *str)
{
	while (*str) {
		UART_DR = (*str++);
	}
}
