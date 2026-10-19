/**
 * @file    dsp.c
 * @brief   RMS, peak and real-power calculations on raw ADC samples.
 */
#include "dsp.h"
#include "common.h"
#include <math.h>

/**
 * @brief  AC-coupled RMS using exact integer sums.
 *         sum((x - mean)^2) = sum(x^2) - (sum x)^2 / n
 * @param  x  Array of raw ADC counts.
 * @param  n  Number of samples.
 * @return RMS of the AC part, in counts.
 */
float calc_rms(const uint16_t *x, uint8_t n)
{
    uint32_t sum = 0, sumsq = 0;
    for (uint8_t k = 0; k < n; k++) {
        uint32_t s = x[k];
        sum   += s;
        sumsq += s * s;
    }
    int64_t acc = (int64_t)sumsq - (int64_t)(((uint64_t)sum * sum) / n);
    return sqrtf((float)acc / n);
}

/**
 * @brief  Sine peak from RMS.
 * @param  rms  RMS value.
 * @return rms * sqrt(2).
 */
float calc_peak(float rms)
{
    return rms * SQRT2;
}

/**
 * @brief  Real power = covariance(v_interp, i) = [sum(w*i) - sum(w)*sum(i)/n] / (2n),
 *         where w = v[k] + v[k+1] (twice the interpolated voltage). Subtracting the
 *         means removes both DC offsets.
 * @param  v  Voltage counts, n + 1 samples.
 * @param  i  Current counts, n samples.
 * @param  n  Number of V/I pairs.
 * @return Real power in count^2 units.
 */
float calc_power(const uint16_t *v, const uint16_t *i, uint8_t n)
{
    uint32_t sum_w = 0, sum_i = 0, sum_wi = 0;
    for (uint8_t k = 0; k < n; k++) {
        uint32_t w  = (uint32_t)v[k] + v[k + 1];
        uint32_t ik = i[k];
        sum_w  += w;
        sum_i  += ik;
        sum_wi += w * ik;
    }
    int64_t cov2 = (int64_t)sum_wi - (int64_t)(((uint64_t)sum_w * sum_i) / n);
    return (float)cov2 / (2.0f * n);
}
