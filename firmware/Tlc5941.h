/*
 * tlc5941.h
 *
 * Created: 03/03/2014 02:29:05 PM
 *  Author: Sebastian Castillo
 */ 

#ifndef _TLC5941_H_
#define _TLC5941_H_

#include <stdint.h>
#include <avr/io.h>

// Pin definition
#ifndef Tlc5941_GSCLK_DDR
#define Tlc5941_GSCLK_DDR DDRB
#endif
#ifndef Tlc5941_GSCLK_PORT
#define Tlc5941_GSCLK_PORT PORTB
#endif
#ifndef Tlc5941_GSCLK_PIN
#define Tlc5941_GSCLK_PIN PB0
#endif

#ifndef Tlc5941_SIN_DDR
#define Tlc5941_SIN_DDR DDRB
#endif
#ifndef Tlc5941_SIN_PORT
#define Tlc5941_SIN_PORT PORTB
#endif
#ifndef Tlc5941_SIN_PIN
#define Tlc5941_SIN_PIN PB3
#endif

#ifndef Tlc5941_SCLK_DDR
#define Tlc5941_SCLK_DDR DDRB
#endif
#ifndef Tlc5941_SCLK_PORT
#define Tlc5941_SCLK_PORT PORTB
#endif
#ifndef Tlc5941_SCLK_PIN
#define Tlc5941_SCLK_PIN PB5
#endif

#ifndef Tlc5941_BLANK_DDR
#define Tlc5941_BLANK_DDR DDRB
#endif
#ifndef Tlc5941_BLANK_PORT
#define Tlc5941_BLANK_PORT PORTB
#endif
#ifndef Tlc5941_BLANK_PIN
#define Tlc5941_BLANK_PIN PB2
#endif

#ifndef Tlc5941_MODE_DDR
#define Tlc5941_MODE_DDR DDRD
#endif
#ifndef Tlc5941_MODE_PORT
#define Tlc5941_MODE_PORT PORTD
#endif
#ifndef Tlc5941_MODE_PIN
#define Tlc5941_MODE_PIN PD7
#endif

#ifndef Tlc5941_XLAT_DDR
#define Tlc5941_XLAT_DDR DDRB
#endif
#ifndef Tlc5941_XLAT_PORT
#define Tlc5941_XLAT_PORT PORTB
#endif
#ifndef Tlc5941_XLAT_PIN
#define Tlc5941_XLAT_PIN PB1
#endif

// Use DC functions?
#ifndef Tlc5941_MANUAL_DC_FUNCS
#define Tlc5941_MANUAL_DC_FUNCS 0
#endif

// Number of TLCs
#ifndef Tlc5941_N
#define Tlc5941_N 1
#endif

// Convenience functions
#define Tlc5941_setOutput(ddr, pin) ((ddr) |= (1 << (pin)))
#define Tlc5941_setLow(port, pin) ((port) &= ~(1 << (pin)))
#define Tlc5941_setHigh(port, pin) ((port) |= (1 << (pin)))
#define Tlc5941_pulse(port, pin) do { \
	Tlc5941_setHigh((port), (pin)); \
	Tlc5941_setLow((port), (pin)); \
} while (0)
#define Tlc5941_outputState(port, pin) ((port) & (1 << (pin)))

// Calculate appropriate data type for size of the dcData array
#if (12 * Tlc5941_N > 255)
#define Tlc5941_dcData_t uint16_t
#else
#define Tlc5941_dcData_t uint8_t
#endif
#define Tlc5941_dcDataSize ((Tlc5941_dcData_t)12 * Tlc5941_N)
// Calculate appropriate data type for size of the gsData array
#if (24 * Tlc5941_N > 255)
#define Tlc5941_gsData_t uint16_t
#else
#define Tlc5941_gsData_t uint8_t
#endif
#define Tlc5941_gsDataSize ((Tlc5941_gsData_t)24 * Tlc5941_N)
// Calculate appropriate data type for number of channels
#if (16 * Tlc5941_N > 255)
#define Tlc5941_channel_t uint16_t
#else
#define Tlc5941_channel_t uint8_t
#endif
#define Tlc5941_numChannels ((Tlc5941_channel_t)16 * Tlc5941_N)

// Data structures that store the dot-correction and grayscale values
extern uint8_t Tlc5941_dcData[Tlc5941_dcDataSize];
extern uint8_t Tlc5941_gsData[Tlc5941_gsDataSize];

// Flag that indicates if grayscale values should be sent in the next ISR execution.
extern volatile uint8_t Tlc5941_gsUpdateFlag;

// Safely sets the update flag
static inline void Tlc5941_SetGSUpdateFlag(void) {
	__asm__ volatile ("" ::: "memory");
	Tlc5941_gsUpdateFlag = 1;
}

void Tlc5941_Init(void);
void Tlc5941_SetGS(Tlc5941_channel_t channel, uint16_t value);
void Tlc5941_SetAllGS(uint16_t value);

#if (Tlc5941_MANUAL_DC_FUNCS)
void Tlc5941_ClockInDC(void);
void Tlc5941_SetDC(Tlc5941_channel_t channel, uint8_t value);
void Tlc5941_SetAllDC(uint8_t value);
#endif

#endif // #ifndef _TLC5941_H_