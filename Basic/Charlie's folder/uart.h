/*
 * uart.h
 *
 * UART (USART) driver header for the ATmega328P (Xplained Mini).
 * Provides initialisation, byte transmission, and helper functions
 * for splitting a 3-digit number into digits and converting a digit
 * to its ASCII character.
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>

// Initialise the USART peripheral (8 data bits, no parity, 1 stop bit,
// asynchronous mode, transmitter only) using the given UBRR value.
void usart_init(uint16_t ubrr);

// Transmit a single byte of data through the USART.
void usart_transmit(uint8_t data);

// Split a 3-digit number (0-999) into its hundreds, tens and units digits.
void extract_digits(uint16_t number, uint8_t *hundreds, uint8_t *tens, uint8_t *units);

// Convert a single digit (0-9) into its ASCII character code.
uint8_t digit_to_ascii(uint8_t digit);

#endif /* UART_H_ */
