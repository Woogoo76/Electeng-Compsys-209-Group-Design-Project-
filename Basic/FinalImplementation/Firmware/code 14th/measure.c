/**
 * @file    measure.c
 * @brief   Scaling of DSP results to volts / mA / watts, and result smoothing.
 */
#include "measure.h"
#include "common.h"
#include "dsp.h"
#include "calib.h"

/**
 * @brief  Window of raw counts -> V rms, I peak (mA), real power (W).
 * @param  v    Voltage counts, n + 1 samples.
 * @param  i    Current counts, n samples.
 * @param  n    Number of V/I pairs.
 * @param  out  Output: measurement result.
 * @return none
 */
void measure_compute(const uint16_t *v, const uint16_t *i, uint8_t n, meas_t *out)
{
    float v_rms = calc_rms(v, n) * VOLTS_PER_COUNT;
    float i_rms = calc_rms(i, n) * AMPS_PER_COUNT;

    /* count^2 -> watts, then undo the amplitude lost by interpolating V */
    float p = calc_power(v, i, n) * VOLTS_PER_COUNT * AMPS_PER_COUNT
              / SAMPLE_SKEW_COS * I_SENSE_SIGN;
    if (p < 0.0f) p = 0.0f;

    out->v_rms   = v_rms;
    out->i_pk_mA = calc_peak(i_rms) * MA_PER_A;
    out->p_w     = p;
}

/**
 * @brief  First-order smoothing of every quantity.
 * @param  acc    In/out: smoothed result.
 * @param  in     New measurement.
 * @param  first  Non-zero on the first call to seed acc with in.
 * @return none
 */
void measure_filter(meas_t *acc, const meas_t *in, uint8_t first)
{
    if (first) { *acc = *in; return; }

    for (uint8_t k = 0; k < MEAS_COUNT; k++) {
        acc->q[k] += (in->q[k] - acc->q[k]) * MEAS_FILTER_ALPHA;
    }
}
