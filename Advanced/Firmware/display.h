/**
 * @file    display.h
 * @brief   4-digit multiplexed 7-segment display (74HC595 + digit enables).
 */
#ifndef DISPLAY_H_
#define DISPLAY_H_
#include <stdint.h>

/** What can be shown. Same order as the MEAS_* indices in measure.h. */
typedef enum { DISP_VOLT = 0, DISP_CURRENT = 1, DISP_POWER = 2, DISP_COUNT = 3 } disp_quantity_t;

/**
 * @brief  Configure the display pins as outputs and switch all digits off.
 * @param  none
 * @return none
 */
void display_init(void);

/**
 * @brief  Format a value as 3 digits + unit letter (V: "14.1U", I: "842A", P: "6.16P")
 *         and queue it. It appears at the start of the next digit scan (double-buffered).
 * @param  q      Which quantity: DISP_VOLT (volts), DISP_CURRENT (mA), DISP_POWER (watts).
 * @param  value  Value in the unit above; clamped to the displayable range.
 * @return none
 */
void display_show_quantity(disp_quantity_t q, float value);

/**
 * @brief  Draw ONE digit (call every 2 ms from the timer ISR): blank all digits, shift
 *         the segment pattern into the 74HC595, latch it, enable the digit, move on.
 * @param  none
 * @return none
 */
void display_isr_step(void);

#endif
