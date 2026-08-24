#ifndef UART_H
#define UART_H

#include <stdint.h>

// Initialise the UART/USART peripheral using the specified UBRR value.
void usart_init(uint16_t ubrr);

// Transmit a single byte of data through the UART.
void usart_transmit(uint8_t data);

// Split a 3-digit number into its hundreds, tens and units digits.
void extract_digits(uint16_t number, uint8_t *hundreds, uint8_t *tens, uint8_t *units);

// Convert a digit from 0-9 into its corresponding ASCII character.
uint8_t digit_to_ascii(uint8_t digit);

#endif