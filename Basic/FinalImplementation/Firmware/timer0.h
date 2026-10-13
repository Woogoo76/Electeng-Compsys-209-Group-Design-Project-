/**
 * @file    timer0.h
 * @brief   1 ms system tick (Timer0). Also steps the 7-segment display every 2 ms.
 */
#ifndef TIMER0_H_
#define TIMER0_H_
#include <stdint.h>

/**
 * @brief  Start Timer0 (1 ms interrupt). Interrupts must be enabled with sei() afterwards.
 * @param  none
 * @return none
 */
void timer0_init(void);

/**
 * @brief  Milliseconds since timer0_init().
 * @param  none
 * @return Elapsed time in ms (32-bit, read atomically).
 */
uint32_t timer0_ms(void);

#endif
