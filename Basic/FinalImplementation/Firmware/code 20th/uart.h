/**
 * @file    uart.h
 * @brief   Blocking transmit-only UART (9600 8N1).
 */
#ifndef UART_H_
#define UART_H_

/**
 * @brief  Configure the USART for 9600 baud, 8 data bits, no parity, 1 stop bit, TX only.
 * @param  none
 * @return none
 */
void uart_init(void);

/**
 * @brief  Send one character (waits until the transmit buffer is free).
 * @param  c  Character (ASCII) to send.
 * @return none
 */
void uart_tx_char(char c);

/**
 * @brief  Send a string (blocking).
 * @param  s  NUL-terminated string to send.
 * @return none
 */
void uart_tx(const char *s);

#endif
