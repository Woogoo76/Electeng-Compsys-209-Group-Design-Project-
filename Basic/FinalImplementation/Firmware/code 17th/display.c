/**
 * @file    display.c
 * @brief   4-digit multiplexed 7-seg driven through a 74HC595 (bit-banged).
 *
 * Segment byte: bit7 = dp ... bit0 = a, shifted MSB first (Lab 6).
 * Layout: [d][d][d][unit]   V: "14.1U"   I: "842A"   P: "6.16P"
 */
#include "display.h"
#include "common.h"
#include "pins.h"
#include <avr/io.h>
#include <util/atomic.h>

#define DISP_DIGITS        4                    /* physical digits                    */
#define DISP_VALUE_DIGITS  (DISP_DIGITS - 1)    /* digits for the number (last = unit) */
#define DISP_MAX_VALUE     999U                 /* largest 3-digit value              */
#define DISP_BUFFERS       2                    /* front + back                       */

#define SEG_DP        0x80                      /* decimal point bit                  */
#define SEG_MSB_MASK  0x80                      /* first bit shifted out              */
#define SEG_BLANK     0x00
#define SEG_U         0x3E                      /* letter U (volts)                   */
#define SEG_A         0x77                      /* letter A (amps)                    */
#define SEG_P         0x73                      /* letter P (power)                   */
#define DP_NONE       0xFF                      /* "no decimal point"                 */

/** Segment pattern for digits 0..9 (Lab 6 QP.1). */
static const uint8_t k_seg_digit[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

/** Digit-enable pins, index 0 = leftmost (Ds1). Active LOW. */
static const pin_t k_digit_pins[DISP_DIGITS] = {
    PIN_REF(DS1), PIN_REF(DS2), PIN_REF(DS3), PIN_REF(DS4)
};

/** How each quantity is shown: scale to a 0..999 integer, where the dp goes, unit glyph. */
typedef struct {
    float   scale;     /* value * scale = integer shown (e.g. volts * 10 -> 145 = "14.5") */
    uint8_t dp_pos;    /* index (0..2) of the digit followed by the dp, or DP_NONE        */
    uint8_t unit;      /* segment pattern of the unit letter                               */
} disp_format_t;

/** Indexed by disp_quantity_t. */
static const disp_format_t k_formats[DISP_COUNT] = {
    { 10.0f,  1,       SEG_U },    /* DISP_VOLT:    14.5 V -> "14.5U" */
    { 1.0f,   DP_NONE, SEG_A },    /* DISP_CURRENT: 842 mA -> "842A"  */
    { 100.0f, 0,       SEG_P },    /* DISP_POWER:   6.16 W -> "6.16P" */
};

/* Front/back buffers: ISR reads front, main writes back, swap on a digit-cycle boundary. */
static volatile uint8_t s_buf[DISP_BUFFERS][DISP_DIGITS];
static volatile uint8_t s_front;       /* which buffer the ISR is drawing           */
static volatile uint8_t s_swap_req;    /* 1 = back buffer holds a new frame         */
static uint8_t s_pos;                  /* digit that will be drawn next (0..3)      */

/**
 * @brief  Set the display pins as outputs; shift-register lines low, all digits off.
 * @param  none
 * @return none
 */
void display_init(void)
{
    PIN_OUTPUT(SH_CP); PIN_OUTPUT(SH_DS); PIN_OUTPUT(SH_ST);
    PIN_LOW(SH_CP);    PIN_LOW(SH_ST);

    for (uint8_t d = 0; d < DISP_DIGITS; d++) {
        pin_make_output(&k_digit_pins[d]);
        pin_write(&k_digit_pins[d], 1);                 /* 1 = digit off */
    }
}

/**
 * @brief  Turn every digit off (prevents ghosting while the segments change).
 * @param  none
 * @return none
 */
static void digits_disable_all(void)
{
    for (uint8_t d = 0; d < DISP_DIGITS; d++) pin_write(&k_digit_pins[d], 1);
}

/**
 * @brief  Turn one digit on.
 * @param  pos  Digit index 0..DISP_DIGITS-1 (0 = leftmost).
 * @return none
 */
static void digit_enable(uint8_t pos)
{
    pin_write(&k_digit_pins[pos], 0);                   /* 0 = digit on */
}

/**
 * @brief  Shift 8 bits into the 74HC595 (MSB first) and latch them to its outputs.
 * @param  pattern  Segment byte: bit7 = dp, bit0 = segment a.
 * @return none
 */
static void shift_out_and_latch(uint8_t pattern)
{
    for (uint8_t bit = SEG_MSB_MASK; bit; bit >>= 1) {
        if (pattern & bit) PIN_HIGH(SH_DS); else PIN_LOW(SH_DS);
        PIN_HIGH(SH_CP);                                /* clock the bit in */
        PIN_LOW(SH_CP);
    }
    PIN_HIGH(SH_ST);                                    /* latch to outputs */
    PIN_LOW(SH_ST);
}

/**
 * @brief  Timer ISR step: draw one digit and advance to the next.
 * @param  none (reads the front buffer and s_pos)
 * @return none (drives the display pins)
 */
void display_isr_step(void)
{
    /* swap buffers only between full scans, so a frame is never half old / half new */
    if (s_pos == 0 && s_swap_req) { s_front ^= 1; s_swap_req = 0; }

    digits_disable_all();                               /* no ghosting                  */
    shift_out_and_latch(s_buf[s_front][s_pos]);         /* segments first (Lab 6 Q1.2)  */
    digit_enable(s_pos);                                /* ... then enable the digit    */

    if (++s_pos >= DISP_DIGITS) s_pos = 0;
}

/**
 * @brief  Copy a finished frame into the back buffer and request a swap.
 * @param  seg  DISP_DIGITS segment bytes, index 0 = leftmost digit.
 * @return none
 */
static void display_write(const uint8_t seg[DISP_DIGITS])
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t back = s_front ^ 1;
        for (uint8_t k = 0; k < DISP_DIGITS; k++) s_buf[back][k] = seg[k];
        s_swap_req = 1;
    }
}

/**
 * @brief  Convert a 0..999 integer into segment patterns (leading zeros blanked).
 * @param  val     Integer to show, 0..DISP_MAX_VALUE.
 * @param  dp_pos  Digit index (0..2) that gets the decimal point, or DP_NONE.
 * @param  unit    Segment pattern for the 4th digit (unit letter).
 * @param  out     Output: DISP_DIGITS segment bytes, index 0 = leftmost.
 * @return none
 */
static void format_7seg(uint16_t val, uint8_t dp_pos, uint8_t unit, uint8_t out[DISP_DIGITS])
{
    uint8_t d[DISP_VALUE_DIGITS];
    for (uint8_t p = DISP_VALUE_DIGITS; p > 0; p--) {   /* split into digits, last first */
        d[p - 1] = (uint8_t)(val % 10);
        val /= 10;
    }

    /* never blank the digit at / after the dp (so 0.05 shows as "0.05") */
    uint8_t limit   = (dp_pos == DP_NONE) ? (DISP_VALUE_DIGITS - 1) : dp_pos;
    uint8_t leading = 1;
    for (uint8_t p = 0; p < DISP_VALUE_DIGITS; p++) {
        if (leading && d[p] == 0 && p < limit) {
            out[p] = SEG_BLANK;
        } else {
            leading = 0;
            out[p] = k_seg_digit[d[p]];
        }
        if (p == dp_pos) out[p] |= SEG_DP;
    }
    out[DISP_VALUE_DIGITS] = unit;
}

/**
 * @brief  Convert a float to a rounded integer clamped to 0..DISP_MAX_VALUE.
 * @param  x  Value (already scaled).
 * @return x rounded to the nearest integer, limited to 0..999.
 */
static uint16_t to_u16_clamped(float x)
{
    if (x < 0.0f) return 0;
    if (x > (float)DISP_MAX_VALUE) return DISP_MAX_VALUE;
    return (uint16_t)(x + ROUND_HALF);
}

/**
 * @brief  Format and queue one quantity (see display.h).
 * @param  q      Quantity to show (selects scale, decimal point and unit letter).
 * @param  value  Value in V, mA or W depending on q.
 * @return none
 */
void display_show_quantity(disp_quantity_t q, float value)
{
    if (q >= DISP_COUNT) return;                        /* ignore invalid selector */

    const disp_format_t *fmt = &k_formats[q];
    uint8_t frame[DISP_DIGITS];
    format_7seg(to_u16_clamped(value * fmt->scale), fmt->dp_pos, fmt->unit, frame);
    display_write(frame);
}
