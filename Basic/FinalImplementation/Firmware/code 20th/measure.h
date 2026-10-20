/**
 * @file    measure.h
 * @brief   Turns a window of raw ADC counts into voltage, peak current and power.
 */
#ifndef MEASURE_H_
#define MEASURE_H_
#include <stdint.h>

/** Index of each quantity inside meas_t.q[] (same order as disp_quantity_t). */
enum { MEAS_V = 0, MEAS_I = 1, MEAS_P = 2, MEAS_COUNT = 3 };

/** One measurement result. Access by name (v_rms ...) or by index (q[MEAS_V] ...). */
typedef union {
    struct {
        float v_rms;     /* source voltage, V rms   */
        float i_pk_mA;   /* load current,   mA peak */
        float p_w;       /* real power,     W       */
    };
    float q[MEAS_COUNT];
} meas_t;

/**
 * @brief  Convert one window of raw ADC counts to engineering units.
 * @param  v    Voltage counts, n + 1 samples.
 * @param  i    Current counts, n samples.
 * @param  n    Number of V/I pairs.
 * @param  out  Output: filled with V rms, I peak (mA) and real power (W).
 * @return none
 */
void measure_compute(const uint16_t *v, const uint16_t *i, uint8_t n, meas_t *out);

/**
 * @brief  Light smoothing so the last display digit does not flicker:
 *         acc = acc + MEAS_FILTER_ALPHA * (in - acc) for every quantity.
 * @param  acc    In/out: smoothed result.
 * @param  in     New measurement.
 * @param  first  Non-zero on the first call: acc is simply set to in.
 * @return none
 */
void measure_filter(meas_t *acc, const meas_t *in, uint8_t first);

#endif
