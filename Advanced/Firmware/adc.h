/**
 * @file    adc.h
 * @brief   ADC acquisition of the voltage (ADC0) and current (ADC1) signals.
 *
 * Usage: adc_init() once, then repeat { adc_start_window(); wait for
 * adc_window_ready(); read the samples; }.
 */
#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>

#define ADC_N_PAIRS 80   /* V/I pairs per window = 20 ms = exactly 10 cycles of 500 Hz */

/**
 * @brief  Configure the ADC (125 kHz clock) and Timer1 (125 us conversion tick).
 * @param  none
 * @return none
 */
void adc_init(void);

/**
 * @brief  Start collecting one window of ADC_N_PAIRS V/I pairs (+1 final V sample).
 *         Collection stops by itself; poll adc_window_ready() for completion.
 * @param  none
 * @return none
 */
void adc_start_window(void);

/**
 * @brief  Has the window started by adc_start_window() finished?
 * @param  none
 * @return 1 when the buffers hold a full window, otherwise 0.
 */
uint8_t adc_window_ready(void);

/**
 * @brief  Voltage samples of the last window.
 * @param  none
 * @return Pointer to ADC_N_PAIRS + 1 raw ADC counts (0..1023). Valid until the next
 *         adc_start_window().
 */
const uint16_t *adc_v_samples(void);

/**
 * @brief  Current samples of the last window. I[k] lies midway between V[k] and V[k+1].
 * @param  none
 * @return Pointer to ADC_N_PAIRS raw ADC counts (0..1023). Valid until the next
 *         adc_start_window().
 */
const uint16_t *adc_i_samples(void);

#endif
