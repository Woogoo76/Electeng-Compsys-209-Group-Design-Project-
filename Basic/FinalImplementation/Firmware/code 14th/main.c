/**
 * @file    main.c
 * @brief   ELECTENG209 energy monitor firmware - state machine only.
 *
 * Data flow:
 *   Timer1 (125 us) -> starts each ADC conversion -> ADC ISR fills the V/I buffers
 *   and stops itself after one window.
 *   main():  INIT -> WAIT (window ready?) -> PROCESS -> REPORT -> REARM -> WAIT ...
 *   Timer0 (1 ms)  -> time base + 7-segment scan, running in parallel.
 */
#include "common.h"
#include "adc.h"
#include "timer0.h"
#include "display.h"
#include "uart.h"
#include "measure.h"
#include "report.h"
#include "readout.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Compile-time check: measurement and display use the same quantity order (V, I, P). */
typedef char assert_meas_matches_display[((int)MEAS_COUNT == (int)DISP_COUNT) ? 1 : -1];

typedef enum { ST_INIT, ST_WAIT, ST_PROCESS, ST_REPORT, ST_REARM } state_t;

static meas_t   g_meas;                              /* smoothed measurement result       */
static float    g_energy_wh;                         /* accumulated energy, Wh            */
static uint32_t g_last_energy_ms;                    /* time of the last energy update    */
static uint32_t g_slot;                              /* 1 s slot that is being shown      */
static uint8_t  g_have_slot;                         /* 1 once a slot has been latched    */
static uint8_t  g_have_data;                         /* 1 once a valid result exists      */
static uint8_t  g_skip_windows = STARTUP_SKIP_WINDOWS;  /* windows still to ignore        */

/**
 * @brief  Initialise every peripheral and enable interrupts.
 * @param  none
 * @return none
 */
static void do_init(void)
{
    uart_init();
    display_init();
    adc_init();
    timer0_init();
    sei();
    g_last_energy_ms = timer0_ms();
}

/**
 * @brief  Turn the finished ADC window into V / I / P, smooth it and add to the energy.
 *         Reads the ADC buffers; updates g_meas, g_energy_wh and g_have_data.
 * @param  none
 * @return none
 */
static void do_process(void)
{
    meas_t m;

    if (g_skip_windows) { g_skip_windows--; return; }   /* ignore start-up transient */

#if DUMP_RAW_FIRST_WINDOW
    if (!g_have_data) {
        report_dump_samples(adc_v_samples(), adc_i_samples(), ADC_N_PAIRS);
        g_last_energy_ms = timer0_ms();      /* don't count the dump time as energy */
    }
#endif

    measure_compute(adc_v_samples(), adc_i_samples(), ADC_N_PAIRS, &m);
    measure_filter(&g_meas, &m, !g_have_data);
    g_have_data = 1;

    uint32_t now = timer0_ms();              /* rectangle-rule energy integration: P * dt */
    g_energy_wh += g_meas.p_w * (float)(now - g_last_energy_ms) / MS_PER_HOUR;
    g_last_energy_ms = now;
}

/**
 * @brief  Latch the result once per SCROLL_MS slot and show it for the whole slot.
 *         At the start of each slot, V -> I -> P takes turns on the 7-segment display
 *         and one UART block is printed. The values do not change again until the
 *         next slot, so the readings stay steady for a full second.
 *         With USE_TEST_VALUES the fixed test values from common.h are used instead
 *         of the measurement.
 * @param  none
 * @return none
 */
static void do_report(void)
{
    uint32_t slot = timer0_ms() / SCROLL_MS;      /* 0, 1, 2 ... one per second */
    float vals[DISP_COUNT];                       /* indexed by disp_quantity_t: V, mA, W */

    if (g_have_slot && slot == g_slot) return;    /* still inside the held second */

#if USE_TEST_VALUES
    vals[DISP_VOLT]    = RMSVoltage;
    vals[DISP_CURRENT] = PeakCurrent;
    vals[DISP_POWER]   = Power;
#else
    if (!g_have_data) return;                     /* nothing valid to show yet */
    for (uint8_t k = 0; k < DISP_COUNT; k++) vals[k] = g_meas.q[k];
#endif

    disp_quantity_t q = (disp_quantity_t)(slot % DISP_COUNT);
    display_show_quantity(q, vals[q]);            /* latched: shown for this whole slot */

    readout_print(vals[DISP_VOLT], vals[DISP_CURRENT], vals[DISP_POWER]);
#if UART_PRINT_ENERGY
    readout_print_energy(g_energy_wh);
#endif

    g_slot = slot;
    g_have_slot = 1;
}

/**
 * @brief  Program entry: run the measurement state machine forever.
 * @param  none
 * @return never returns
 */
int main(void)
{
    state_t state = ST_INIT;

    for (;;) {
        switch (state) {
        case ST_INIT:    do_init(); adc_start_window(); state = ST_WAIT;   break;
        case ST_WAIT:    if (adc_window_ready()) state = ST_PROCESS;       break;
        case ST_PROCESS: do_process();           state = ST_REPORT;        break;
        case ST_REPORT:  do_report();            state = ST_REARM;         break;
        case ST_REARM:   adc_start_window();     state = ST_WAIT;          break;
        }
    }
}
