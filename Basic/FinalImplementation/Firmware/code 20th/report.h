/**
 * @file    report.h
 * @brief   Test-record helper: dump raw ADC samples over UART.
 */
#ifndef REPORT_H_
#define REPORT_H_
#include <stdint.h>

/**
 * @brief  Print one "v, i" line of ADC counts per sample pair (copy/paste into Excel).
 * @param  v  Voltage counts (at least n entries).
 * @param  i  Current counts (at least n entries).
 * @param  n  Number of pairs to print.
 * @return none (text sent over UART)
 */
void report_dump_samples(const uint16_t *v, const uint16_t *i, uint8_t n);

#endif
