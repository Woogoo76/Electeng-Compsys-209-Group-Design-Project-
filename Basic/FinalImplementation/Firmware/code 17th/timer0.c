/**
 * @file    timer0.c
 * @brief   Timer0 CTC tick: millisecond counter + display scan.
 */
#include "timer0.h"
#include "common.h"
#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define TIMER0_PRESCALER 8UL                                   /* CS01 -> clk/8            */
#define TIMER0_TOP       ((F_CPU / TIMER0_PRESCALER / 1000UL) - 1UL)  /* 1 ms tick (=249)  */
#define DISPLAY_STEP_MS  2                                     /* 4 digits x 2 ms = 125 Hz */

static volatile uint32_t s_ms;     /* milliseconds since init */
static uint8_t s_div;              /* ticks since last display step */

/**
 * @brief  Configure Timer0 for a 1 ms CTC interrupt.
 * @param  none
 * @return none
 */
void timer0_init(void)
{
    TCCR0A = (1 << WGM01);               /* CTC mode */
    OCR0A  = (uint8_t)TIMER0_TOP;
    TIMSK0 = (1 << OCIE0A);
    TCCR0B = (1 << CS01);                /* start, prescaler 8 */
}

/**
 * @brief  Read the millisecond counter safely (32-bit read is not atomic on AVR).
 * @param  none
 * @return Milliseconds since timer0_init().
 */
uint32_t timer0_ms(void)
{
    uint32_t t;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { t = s_ms; }
    return t;
}

/**
 * @brief  1 ms tick ISR: count time and step the display every DISPLAY_STEP_MS.
 *         ISR_NOBLOCK re-enables interrupts so the ADC ISR (tight timing) can preempt
 *         the display bit-banging.
 * @param  none
 * @return none (writes s_ms, calls display_isr_step())
 */
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
    s_ms++;
    if (++s_div >= DISPLAY_STEP_MS) {
        s_div = 0;
        display_isr_step();
    }
}
