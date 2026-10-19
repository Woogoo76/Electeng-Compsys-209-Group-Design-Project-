/**
 * @file    report.c
 * @brief   Raw ADC sample dump for the test record.
 */
#include "report.h"
#include "uart.h"

#define REPORT_LINE_LEN  16          /* "1023, 1023\r\n" + NUL fits comfortably */
#define NEWLINE          "\r\n"

/**
 * @brief  Append a string to a buffer.
 * @param  p  Write position in the buffer.
 * @param  s  NUL-terminated string to copy (the NUL is not copied).
 * @return New write position (just after the copied text).
 */
static char *put_str(char *p, const char *s)
{
    while (*s) *p++ = *s++;
    return p;
}

/**
 * @brief  Append an unsigned number in decimal.
 * @param  p    Write position in the buffer.
 * @param  val  Number to write (0..65535).
 * @return New write position (just after the digits).
 */
static char *put_uint(char *p, uint16_t val)
{
    char tmp[5];                     /* 65535 has 5 digits */
    uint8_t n = 0;
    do {
        tmp[n++] = (char)('0' + val % 10);
        val /= 10;
    } while (val);
    while (n) *p++ = tmp[--n];
    return p;
}

/**
 * @brief  Send "v, i" for each pair, one pair per line.
 * @param  v  Voltage counts.
 * @param  i  Current counts.
 * @param  n  Number of pairs.
 * @return none
 */
void report_dump_samples(const uint16_t *v, const uint16_t *i, uint8_t n)
{
    char line[REPORT_LINE_LEN];
    for (uint8_t k = 0; k < n; k++) {
        char *p = line;
        p = put_uint(p, v[k]);
        p = put_str(p, ", ");
        p = put_uint(p, i[k]);
        p = put_str(p, NEWLINE);
        *p = '\0';
        uart_tx(line);
    }
}
