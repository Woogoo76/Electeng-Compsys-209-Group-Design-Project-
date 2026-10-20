/**
 * @file    adc.c
 * @brief   ADC acquisition. A Timer1 interrupt picks the channel and starts each
 *          conversion; the ADC interrupt stores the result.
 *
 * Samples alternate V, I, V, I ... V so every I sample sits between two V samples.
 * ADC clock 125 kHz (2 MHz / 16); one conversion every 125 us = 8 kHz total
 * (spec: <= 10 kHz). The channel is set immediately before the conversion is
 * started, so there is no mux/trigger timing ambiguity.
 */
#include "adc.h"
#include "common.h"
#include "pins.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Sample "slots": index into the buffers and the mux table below */
#define ADC_SLOT_V     0
#define ADC_SLOT_I     1
#define ADC_NUM_SLOTS  2

#define ADC_TRIGGER_US 125UL                                        /* time between conversions */
#define ADC_TIMER_TOP  ((F_CPU / 1000000UL) * ADC_TRIGGER_US - 1UL) /* Timer1 OCR1A (=249)      */
#define ADMUX_BASE     (1 << REFS0)                                 /* AVcc reference           */
#define ADCSRA_BASE    ((1 << ADEN) | (1 << ADPS2))                 /* enabled, prescaler 16    */

/** ADMUX value for each slot (reference + channel). */
static const uint8_t k_admux[ADC_NUM_SLOTS] = {
    (uint8_t)(ADMUX_BASE | ADC_CH_V),
    (uint8_t)(ADMUX_BASE | ADC_CH_I)
};

/** g_samples[ADC_SLOT_V][0..N] = voltage, g_samples[ADC_SLOT_I][0..N-1] = current. */
static uint16_t g_samples[ADC_NUM_SLOTS][ADC_N_PAIRS + 1];
static volatile uint8_t s_ready;     /* 1 = window complete                          */
static volatile uint8_t s_running;   /* 1 = a window is being collected              */
static volatile uint8_t s_next;      /* slot of the NEXT conversion                  */
static volatile uint8_t s_cur;       /* slot of the conversion in progress           */
static volatile uint8_t s_k;         /* index of the pair being collected            */

/**
 * @brief  Configure the ADC and Timer1.
 * @param  none
 * @return none
 */
void adc_init(void)
{
    ADMUX  = k_admux[ADC_SLOT_V];
    ADCSRA = ADCSRA_BASE;
    DIDR0  = (1 << ADC0D) | (1 << ADC1D);          /* digital input buffers off */

    /* Timer1: CTC, TOP = OCR1A, no prescaler (0.5 us per tick) */
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10);
    OCR1A  = (uint16_t)ADC_TIMER_TOP;
    TIMSK1 = (1 << OCIE1A);
#if DEBUG_PINS
    PIN_OUTPUT(DBG_SOC);
    PIN_OUTPUT(DBG_EOC);
#endif
}

/**
 * @brief  Begin collecting a new window of samples.
 * @param  none
 * @return none
 */
void adc_start_window(void)
{
    s_k = 0;
    s_next = ADC_SLOT_V;
    s_ready = 0;
    s_running = 1;
}

/**
 * @brief  Is the current window complete?
 * @param  none
 * @return 1 if complete, 0 otherwise.
 */
uint8_t adc_window_ready(void) { return s_ready; }

/**
 * @brief  Voltage samples (ADC_N_PAIRS + 1 raw counts).
 * @param  none
 * @return Pointer to the voltage buffer.
 */
const uint16_t *adc_v_samples(void) { return g_samples[ADC_SLOT_V]; }

/**
 * @brief  Current samples (ADC_N_PAIRS raw counts).
 * @param  none
 * @return Pointer to the current buffer.
 */
const uint16_t *adc_i_samples(void) { return g_samples[ADC_SLOT_I]; }

/**
 * @brief  Timer1 compare ISR, every ADC_TRIGGER_US: start a conversion on the next slot.
 * @param  none (reads s_running / s_next)
 * @return none (writes s_cur, s_next, ADMUX and starts the ADC)
 */
ISR(TIMER1_COMPA_vect)
{
    if (!s_running) return;
#if DEBUG_PINS
    PIN_TOGGLE(DBG_SOC);
#endif
    s_cur = s_next;
    s_next ^= 1;                                          /* alternate V <-> I */
    ADMUX = k_admux[s_cur];
    ADCSRA = ADCSRA_BASE | (1 << ADIE) | (1 << ADSC);     /* plain write: never clears ADIF by RMW */
}

/**
 * @brief  ADC conversion-complete ISR: store the result, stop after the last V sample.
 * @param  none (reads the ADC data register and s_cur)
 * @return none (writes g_samples, s_k, s_ready, s_running)
 */
ISR(ADC_vect)
{
    uint16_t result = ADC;
    uint8_t  slot   = s_cur;
#if DEBUG_PINS
    PIN_TOGGLE(DBG_EOC);
#endif
    g_samples[slot][s_k] = result;

    if (slot == ADC_SLOT_I) {
        s_k++;                                     /* a V/I pair is complete */
    } else if (s_k >= ADC_N_PAIRS) {               /* got V[N]: window complete */
        s_running = 0;
        ADCSRA = ADCSRA_BASE;                      /* ADIE off */
        s_ready = 1;
    }
}
