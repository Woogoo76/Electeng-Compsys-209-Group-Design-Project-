#include "uart.h"

void usart_init(uint16_t ubrr)
{
	// Set baud rate registers
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	// Clear UCSR0A (ensure normal speed operation)
	UCSR0A = 0x00;

	// Enable Transmitter
	UCSR0B = (1 << TXEN0);

	// Frame format: 8 data bits, no parity, 1 stop bit (8N1)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void usart_transmit(uint8_t data)
{
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)))
	{
		;
	}

	// Put data into buffer, sends the data
	UDR0 = data;
}

void extract_digits(uint16_t number, uint8_t *hundreds, uint8_t *tens, uint8_t *units)
{
	*hundreds = (number / 100) % 10;
	*tens = (number / 10) % 10;
	*units = number % 10;
}

uint8_t digit_to_ascii(uint8_t digit)
{
	return digit + 48; // Convert 0-9 to '0'-'9'
}