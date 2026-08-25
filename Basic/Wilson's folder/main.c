#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>

// System clock frequency is 2 MHz.
#define F_CPU 2000000UL

// UBRR value for 9600 baud at a 2 MHz clock.
#define UBRR_VALUE 12

// Project values.
#define RMSVoltage   14.5
#define PeakCurrent  125
#define Power        1.60

// Transmit a complete string through the UART.
void print_string(const char *str)
{
	// Continue transmitting until the null character is reached.
	while (*str != '\0')
	{
		// Transmit the current character.
		usart_transmit((uint8_t)*str);

		// Move to the next character in the string.
		str++;
	}
}

int main(void)
{
	// Variables used to store the hundreds, tens and units digits.
	uint8_t hundreds, tens, units;
	
	// Initialise the UART.
	usart_init(UBRR_VALUE);
	
	while (1)
	{
		// Extract the digits from RMSVoltage.
		// Multiplying by 10 removes the decimal point:
		// 14.5 becomes 145.
		extract_digits((uint16_t)(RMSVoltage * 10), &hundreds, &tens, &units);

		// Print the RMS Voltage label.
		print_string("RMS Voltage is: ");

		// Print the hundreds digit.
		usart_transmit(digit_to_ascii(hundreds));

		// Print the tens digit.
		usart_transmit(digit_to_ascii(tens));

		// Print the decimal point.
		usart_transmit('.');

		// Print the units digit.
		usart_transmit(digit_to_ascii(units));

		// Move to a new line.
		print_string("\r\n");


		// Extract the digits from PeakCurrent.
		// 125 is already an integer.
		extract_digits((uint16_t)PeakCurrent, &hundreds, &tens, &units);

		// Print the Peak Current label.
		print_string("Peak Current is: ");

		// Print the hundreds digit.
		usart_transmit(digit_to_ascii(hundreds));

		// Print the tens digit.
		usart_transmit(digit_to_ascii(tens));

		// Print the units digit.
		usart_transmit(digit_to_ascii(units));

		// Move to a new line.
		print_string("\r\n");


		// Extract the digits from Power.
		// Multiplying by 100 removes the decimal point:
		// 1.60 becomes 160.
		extract_digits((uint16_t)(Power * 100), &hundreds, &tens, &units);

		// Print the Power label.
		print_string("Power is: ");

		// Print the hundreds digit.
		usart_transmit(digit_to_ascii(hundreds));

		// Print the decimal point.
		usart_transmit('.');

		// Print the tens digit.
		usart_transmit(digit_to_ascii(tens));

		// Print the units digit.
		usart_transmit(digit_to_ascii(units));

		// Move to a new line.
		print_string("\r\n");
		
		// Wait 1 second before printing the values again.
		_delay_ms(1000);
	}
}