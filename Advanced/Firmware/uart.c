/**
 * @file    uart.c
 * @brief   Blocking transmit-only UART driver.
 */
#include "uart.h"
#include "common.h"
#include <avr/io.h>

#define UART_UBRR ((F_CPU / (16UL * UART_BAUD)) - 1UL)   /* = 12 at 2 MHz / 9600 (Lab 2) */

/**
 * @brief  Set baud rate and frame format, enable the transmitter.
 * @param  none
 * @return none
 */
void uart_init(void)
{
    UBRR0H = (uint8_t)(UART_UBRR >> 8);
    UBRR0L = (uint8_t)UART_UBRR;
    UCSR0A = 0;
    UCSR0B = (1 << TXEN0);                           /* transmitter only */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);          /* 8 data bits, no parity, 1 stop */
}

/**
 * @brief  Wait for the transmit buffer to be empty, then send one character.
 * @param  c  Character to send.
 * @return none
 */
void uart_tx_char(char c)
{
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = c;
}

/**
 * @brief  Send every character of a string.
 * @param  s  NUL-terminated string.
 * @return none
 */
void uart_tx(const char *s)
{
    while (*s) uart_tx_char(*s++);
}
