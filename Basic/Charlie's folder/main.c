/*
 * main.c
 *
 * Prints RMS Voltage, Peak Current and Power to a terminal (PuTTY/RealTerm)
 * via UART every 1 second, using the driver in uart.h / uart.c.
 *
 * RMSVoltage, PeakCurrent and Power are defined below exactly as they'd be
 * measured/used in the project (e.g. 14.5, 125, 1.60). Since these are
 * compile-time constants, multiplying them by 10/100 to get an integer
 * with the decimal point removed is evaluated by the compiler at compile
 * time -- no runtime floating-point math happens on the microcontroller.
 */
 
#define F_CPU 2000000UL
 
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
 
#define UBRR_VALUE 12 // 9600 baud @ 2 MHz system clock (Lab 2, Q2.2)
 
// Project values, defined in their natural form (format: XY.Z, XYZ, X.YZ)
#define RMSVoltage   14.5
#define PeakCurrent  125
#define Power        1.60
 
// Sends each character of a null-terminated string via usart_transmit
void print_string(const char *str)
{
    while (*str != '\0')
    {
        usart_transmit((uint8_t)*str);
        str++;
    }
}
 
int main(void)
{
    uint8_t hundreds, tens, units;
 
    usart_init(UBRR_VALUE);
 
    while (1)
    {
        // --- RMS Voltage: XY.Z ---
        extract_digits((uint16_t)(RMSVoltage * 10), &hundreds, &tens, &units);
        print_string("RMS Voltage is: ");
        usart_transmit(digit_to_ascii(hundreds));
        usart_transmit(digit_to_ascii(tens));
        usart_transmit('.');
        usart_transmit(digit_to_ascii(units));
        print_string("\r\n");
 
        // --- Peak Current: XYZ ---
        extract_digits((uint16_t)PeakCurrent, &hundreds, &tens, &units);
        print_string("Peak Current is: ");
        usart_transmit(digit_to_ascii(hundreds));
        usart_transmit(digit_to_ascii(tens));
        usart_transmit(digit_to_ascii(units));
        print_string("\r\n");
 
        // --- Power: X.YZ ---
        extract_digits((uint16_t)(Power * 100), &hundreds, &tens, &units);
        print_string("Power is: ");
        usart_transmit(digit_to_ascii(hundreds));
        usart_transmit('.');
        usart_transmit(digit_to_ascii(tens));
        usart_transmit(digit_to_ascii(units));
        print_string("\r\n");
 
        _delay_ms(1000);
    }
}