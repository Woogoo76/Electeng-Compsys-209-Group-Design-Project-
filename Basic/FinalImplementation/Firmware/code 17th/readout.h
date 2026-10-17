/**
 * @file    readout.h
 * @brief   UART text output in the fixed assessment format (zero-padded):
 *            RMS Voltage is: XX.X
 *            Peak Current is: XXX
 *            Power is: X.XX
 *            (blank line)
 */
#ifndef READOUT_H_
#define READOUT_H_

/**
 * @brief  Print the three lines above followed by a blank line.
 * @param  rms_voltage   Voltage in V rms (0..99.9).
 * @param  peak_current  Current in mA peak (0..999).
 * @param  power         Real power in W (0..9.99).
 * @return none (text sent over UART; out-of-range values are clamped)
 */
void readout_print(float rms_voltage, float peak_current, float power);

/**
 * @brief  Optional extra block: "Energy is: XXXX.XXX Wh" followed by a blank line
 *         (enabled with UART_PRINT_ENERGY in common.h).
 * @param  energy_wh  Accumulated energy in Wh (0..9999.999).
 * @return none (text sent over UART)
 */
void readout_print_energy(float energy_wh);

#endif
