/**
 * @file    pins.h
 * @brief   Pin map (from Overall_Project_Proteus.pdsprj) and tiny pin helpers.
 *
 * A pin "X" is described by three macros: X_PORT, X_DDR and X_BIT.
 *  - Compile-time pins use PIN_HIGH/PIN_LOW/... (fast, single instruction).
 *  - Groups of identical pins (the 4 digit enables) use a pin_t table + loop.
 */
#ifndef PINS_H_
#define PINS_H_
#include <avr/io.h>
#include <stdint.h>

/* ---- Single-pin macros: argument p is the pin name, e.g. PIN_HIGH(SH_CP) ---- */
#define PIN_OUTPUT(p) ((p##_DDR)  |=  (1 << (p##_BIT)))   /* make pin an output     */
#define PIN_HIGH(p)   ((p##_PORT) |=  (1 << (p##_BIT)))   /* drive pin high         */
#define PIN_LOW(p)    ((p##_PORT) &= ~(1 << (p##_BIT)))   /* drive pin low          */
#define PIN_TOGGLE(p) ((p##_PIN)   =  (1 << (p##_BIT)))   /* toggle (write 1 to PINx) */

/* ---- Pin tables: lets a loop handle several similar pins ---- */
/** One I/O pin: its data-direction register, port register and bit mask. */
typedef struct {
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    uint8_t           mask;
} pin_t;

/** Builds a pin_t initialiser from the pin name, e.g. PIN_REF(DS1). */
#define PIN_REF(p) { &(p##_DDR), &(p##_PORT), (uint8_t)(1U << (p##_BIT)) }

/**
 * @brief  Make a pin an output.
 * @param  pin  Pin to configure (pointer to a pin_t table entry).
 * @return none
 */
static inline void pin_make_output(const pin_t *pin) { *pin->ddr |= pin->mask; }

/**
 * @brief  Drive a pin high or low.
 * @param  pin   Pin to drive.
 * @param  high  Non-zero = logic 1 (5 V), zero = logic 0 (0 V).
 * @return none
 */
static inline void pin_write(const pin_t *pin, uint8_t high)
{
    if (high) *pin->port |= pin->mask;
    else      *pin->port &= (uint8_t)~pin->mask;
}

/* ---- ADC channels (Lab 4: ADC0 = voltage signal, ADC1 = current signal) ---- */
#define ADC_CH_V 1
#define ADC_CH_I 0

/* ---- 74HC595 shift register (Q0 = segment a ... Q7 = dp) ---- */
#define SH_CP_PORT PORTC
#define SH_CP_DDR  DDRC
#define SH_CP_BIT  PC3
#define SH_DS_PORT PORTC
#define SH_DS_DDR  DDRC
#define SH_DS_BIT  PC4
#define SH_ST_PORT PORTC
#define SH_ST_DDR  DDRC
#define SH_ST_BIT  PC5

/* ---- Digit enables (common cathodes, active LOW). Ds1 = leftmost digit ---- */
#define DS1_PORT PORTD
#define DS1_DDR  DDRD
#define DS1_BIT  PD4
#define DS2_PORT PORTD
#define DS2_DDR  DDRD
#define DS2_BIT  PD5
#define DS3_PORT PORTD
#define DS3_DDR  DDRD
#define DS3_BIT  PD6
#define DS4_PORT PORTD
#define DS4_DDR  DDRD
#define DS4_BIT  PD7

/* ---- Debug pins (only used when DEBUG_PINS = 1) ----
 * SoC toggles PB2 (unconnected in the model - wire it to a scope channel),
 * EoC toggles the LED pin PB5 (already on Proteus scope channel D). */
#define DBG_SOC_PORT PORTB
#define DBG_SOC_DDR  DDRB
#define DBG_SOC_PIN  PINB
#define DBG_SOC_BIT  PB2
#define DBG_EOC_PORT PORTB
#define DBG_EOC_DDR  DDRB
#define DBG_EOC_PIN  PINB
#define DBG_EOC_BIT  PB5

#endif
