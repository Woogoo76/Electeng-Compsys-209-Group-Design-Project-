/**
 * @file    dsp.h
 * @brief   Pure signal-processing functions (outputs depend only on inputs).
 *          All results are in ADC counts, not volts/amps - see measure.c for scaling.
 */
#ifndef DSP_H_
#define DSP_H_
#include <stdint.h>

/**
 * @brief  AC-coupled RMS (mean removed, so any DC offset is ignored).
 * @param  x  Array of raw ADC counts.
 * @param  n  Number of samples in x (ideally a whole number of cycles).
 * @return RMS of the AC part, in ADC counts.
 */
float calc_rms(const uint16_t *x, uint8_t n);

/**
 * @brief  Peak value of a sine wave from its RMS (peak = sqrt(2) * rms).
 *         Used instead of picking the largest sample: with 8 samples per cycle that
 *         would under-read by up to 7.6 %.
 * @param  rms  RMS value (any unit).
 * @return Peak value, same unit as rms.
 */
float calc_peak(float rms);

/**
 * @brief  Real power from interleaved V/I samples (mean of v*i, DC removed).
 *         i[k] is taken midway between v[k] and v[k+1], so V is interpolated by
 *         averaging those two samples. The result is NOT yet corrected for the
 *         amplitude lost by that averaging (see SAMPLE_SKEW_COS in calib.h).
 * @param  v  Voltage counts, n + 1 samples.
 * @param  i  Current counts, n samples.
 * @param  n  Number of V/I pairs.
 * @return Real power in count^2 units.
 */
float calc_power(const uint16_t *v, const uint16_t *i, uint8_t n);

#endif
