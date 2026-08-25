#include "uart.h"
#include <avr/io.h>

// Initialise the UART/USART peripheral.
void usart_init(uint16_t ubrr) {

	// Clear the USART status register.
	UCSR0A = 0b00000000;

	// Set the baud rate using the UBRR value.
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	// Enable the USART transmitter.
	UCSR0B = (1 << TXEN0);

	// Set the UART frame format:
	// 8 data bits, no parity, 1 stop bit.
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmit a single byte through the UART.
void usart_transmit(uint8_t data) {

	// Wait until the transmit data register is empty.
	while ((UCSR0A & (1 << UDRE0)) == 0) {
		;
	}

	// Place the data into the transmit register to begin transmission.
	UDR0 = data;
}

// Extract the hundreds, tens and units digits from a number.
void extract_digits(uint16_t number, uint8_t *hundreds, uint8_t *tens, uint8_t *units)
{
	// Extract the hundreds digit.
	*hundreds = (number / 100) % 10;

	// Extract the tens digit.
	*tens = (number / 10) % 10;

	// Extract the units digit.
	*units = number % 10;
}

// Convert a digit from 0-9 into its ASCII value.
uint8_t digit_to_ascii(uint8_t digit)
{
	// ASCII value of '0' is 48, so adding 48 converts
	// the numerical digit into its ASCII character.
	return digit + 48;
}