/**
 * @file    readout.c
 * @brief   Formats numbers as fixed-width text and sends them over UART.
 *          One table describes every printed line, so adding a line = adding a table row.
 */
#include "readout.h"
#include "common.h"
#include "uart.h"
#include <stdint.h>

#define READOUT_FIELDS    3                      /* voltage, current, power lines */
#define READOUT_MAX_DIGITS 10                    /* longest number we format      */
#define NEWLINE           "\r\n"

/** One printed line: "<label><value>". */
typedef struct {
    const char *label;       /* text before the number                           */
    float       scale;       /* value * scale -> integer of (int_digits+decimals) */
    uint8_t     int_digits;  /* digits before the decimal point                  */
    uint8_t     decimals;    /* digits after the decimal point                   */
} field_t;

/** Order must match the arguments of readout_print(). */
static const field_t k_fields[READOUT_FIELDS] = {
    { "RMS Voltage is: ",  10.0f,   2, 1 },      /* XX.X */
    { "Peak Current is: ",  1.0f,   3, 0 },      /* XXX  */
    { "Power is: ",       100.0f,   1, 2 },      /* X.XX */
};

static const field_t k_energy_field = { "Energy is: ", 1000.0f, 4, 3 };   /* XXXX.XXX */

/**
 * @brief  Send an integer as int_digits.decimals digits, zero padded
 *         (e.g. val 145, 2 int digits, 1 decimal -> "14.5").
 * @param  val         Integer to print (must fit in int_digits + decimals digits).
 * @param  int_digits  Digits before the decimal point.
 * @param  decimals    Digits after the decimal point (0 = no decimal point).
 * @return none (characters sent over UART)
 */
static void tx_fixed(uint32_t val, uint8_t int_digits, uint8_t decimals)
{
    char d[READOUT_MAX_DIGITS];
    uint8_t total = (uint8_t)(int_digits + decimals);

    for (uint8_t k = total; k > 0; k--) {          /* digits, last first */
        d[k - 1] = (char)('0' + val % 10);
        val /= 10;
    }
    for (uint8_t k = 0; k < total; k++) {
        if (decimals && k == int_digits) uart_tx_char('.');
        uart_tx_char(d[k]);
    }
}

/**
 * @brief  Convert a float to a rounded integer, clamped so it always fits the digit count.
 * @param  x       Value to convert (negative values give 0).
 * @param  mult    Scale factor (10 for one decimal, 100 for two ...).
 * @param  digits  Total digits available; the result is limited to 10^digits - 1.
 * @return Rounded, clamped integer.
 */
static uint32_t scale_clamped(float x, float mult, uint8_t digits)
{
    uint32_t max = 1;
    for (uint8_t k = 0; k < digits; k++) max *= 10UL;
    max -= 1;

    if (x < 0.0f) return 0;
    float s = x * mult + ROUND_HALF;
    return (s > (float)max) ? max : (uint32_t)s;
}

/**
 * @brief  Send "<label><value>" for one table row (no line ending).
 * @param  f      Row describing label, scale and digit counts.
 * @param  value  Value to print, in the unit that matches f->scale.
 * @return none (text sent over UART)
 */
static void print_field(const field_t *f, float value)
{
    uart_tx(f->label);
    tx_fixed(scale_clamped(value, f->scale, (uint8_t)(f->int_digits + f->decimals)),
             f->int_digits, f->decimals);
}

/**
 * @brief  Print voltage, current and power lines (see readout.h).
 * @param  rms_voltage   V rms.
 * @param  peak_current  mA peak.
 * @param  power         W.
 * @return none
 */
void readout_print(float rms_voltage, float peak_current, float power)
{
    const float values[READOUT_FIELDS] = { rms_voltage, peak_current, power };

    for (uint8_t k = 0; k < READOUT_FIELDS; k++) {
        print_field(&k_fields[k], values[k]);
        uart_tx(NEWLINE);
    }
    uart_tx(NEWLINE);                              /* blank line between blocks */
}

/**
 * @brief  Print the energy line (see readout.h).
 * @param  energy_wh  Energy in Wh.
 * @return none
 */
void readout_print_energy(float energy_wh)
{
    print_field(&k_energy_field, energy_wh);
    uart_tx(" Wh" NEWLINE NEWLINE);
}
