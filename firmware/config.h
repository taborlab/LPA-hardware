/*
 * config.h
 *
 * Created: 03/03/2014 02:29:05 PM
 *  Author: Sebastian Castillo
 */ 

// Global configuration variables
#define F_CPU 16000000UL

// Configuration for Tlc5941 module
#define Tlc5941_GSCLK_DDR DDRB
#define Tlc5941_GSCLK_PORT PORTB
#define Tlc5941_GSCLK_PIN PB0

#define Tlc5941_SIN_DDR DDRB
#define Tlc5941_SIN_PORT PORTB
#define Tlc5941_SIN_PIN PB3

#define Tlc5941_SCLK_DDR DDRB
#define Tlc5941_SCLK_PORT PORTB
#define Tlc5941_SCLK_PIN PB5

#define Tlc5941_BLANK_DDR DDRB
#define Tlc5941_BLANK_PORT PORTB
#define Tlc5941_BLANK_PIN PB2

#define Tlc5941_MODE_DDR DDRD
#define Tlc5941_MODE_PORT PORTD
#define Tlc5941_MODE_PIN PD4

#define Tlc5941_XLAT_DDR DDRB
#define Tlc5941_XLAT_PORT PORTB
#define Tlc5941_XLAT_PIN PB1

#define Tlc5941_N 3

#define Tlc5941_MANUAL_DC_FUNCS 1