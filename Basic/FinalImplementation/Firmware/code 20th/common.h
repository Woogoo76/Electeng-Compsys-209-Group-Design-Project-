/**
 * @file    common.h
 * @brief   Project-wide build options, test values and shared constants.
 *
 * Everything that is a "setting" or a repeated number lives here (or in calib.h /
 * pins.h) as a named macro, so no magic numbers appear in the .c files.
 */
#ifndef COMMON_H_
#define COMMON_H_

#ifndef F_CPU
#define F_CPU 2000000UL          /* MCU system clock in Hz (design spec) */
#endif
#define UART_BAUD 9600UL         /* UART baud rate (design spec) */

/* ---- Build options (1 = on, 0 = off) ---- */
#define DEBUG_PINS            0  /* 1: SoC (PB2) + EoC (PB5) pin toggles for the test record */
#define DUMP_RAW_FIRST_WINDOW 0  /* 1: print first raw ADC window over UART (paste into Excel) */
#define UART_PRINT_ENERGY     1 /* 1: also print "Energy is: ..." after the three lines */

/* UART assessment test: 1 = print/display the fixed values below instead of measurements.
 * Change the three values (keep the format) to test. */
#define USE_TEST_VALUES       0
#define RMSVoltage   14.5        /* XX.X */
#define PeakCurrent  125         /* XXX  */
#define Power        1.60        /* X.XX */

/* ---- Timing of the main loop ---- */
#define SCROLL_MS            1000UL   /* each quantity is latched and shown this long;
                                         the UART block is printed once per slot too   */
#define STARTUP_SKIP_WINDOWS 2        /* ignore first ADC windows (Proteus start-up) */

/* ---- Shared constants ---- */
#define SQRT2        1.41421356f      /* peak = SQRT2 * rms for a sine wave          */
#define ROUND_HALF   0.5f             /* added before truncating a float to round it */
#define MA_PER_A     1000.0f          /* milliamps per amp                           */
#define MS_PER_HOUR  3.6e6f           /* milliseconds per hour (energy integration)  */

/** Number of elements in a C array. */
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

#endif
