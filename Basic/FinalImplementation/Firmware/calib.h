/**
 * @file    calib.h
 * @brief   Analogue front-end constants (from Labs 1 & 3) and measurement tuning.
 *
 * Edit these to match what you BUILT, then fine-trim V_TRIM / I_TRIM against a
 * multimeter. The 2.1 V / 2.5 V offset is not needed: it is removed in software.
 */
#ifndef CALIB_H_
#define CALIB_H_

#define ADC_VREF_V    5.0f                   /* ADC reference (AVcc), volts            */
#define ADC_STEPS     1024.0f                /* 10-bit ADC: counts per full scale      */

#define VDIV_RA_OHM   56330.0f               /* voltage divider top resistor (Lab 1)   */
#define VDIV_RB_OHM   2703.0f                /* voltage divider bottom resistor        */
#define V_AMP_GAIN    1.0f                   /* voltage diff-amp gain (Lab 3)          */

#define RSHUNT_OHM    0.665f                   /* current shunt (Lab 1)                  */
#define I_AMP_GAIN    (68170.0f / 29900.0f)  /* current diff-amp gain R2/R1 (Lab 3)    */
#define I_SENSE_SIGN  1.0f                   /* set -1.0f if power reads negative      */

#define V_TRIM        1.0f                   /* multimeter calibration factor, voltage */
#define I_TRIM        1.04f                   /* multimeter calibration factor, current */

/* Source volts / amps per ADC count (AC-coupled, so DC offset drops out) */
#define VOLTS_PER_COUNT ((ADC_VREF_V / ADC_STEPS) * ((VDIV_RA_OHM + VDIV_RB_OHM) / VDIV_RB_OHM) / V_AMP_GAIN * V_TRIM)
#define AMPS_PER_COUNT  ((ADC_VREF_V / ADC_STEPS) / (I_AMP_GAIN * RSHUNT_OHM) * I_TRIM)

/* V and I are sampled 125 us apart (interleaved). Averaging two neighbouring V
 * samples to line up with I loses cos(2*pi*500Hz*125us) = cos(22.5 deg) of amplitude. */
#define SAMPLE_SKEW_COS 0.92388f

/* Smoothing of the displayed result: new = old + ALPHA * (measured - old) */
#define MEAS_FILTER_ALPHA 0.25f

#endif