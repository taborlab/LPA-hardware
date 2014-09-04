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

#define Tlc5941_XLAT_DDR DDRB
#define Tlc5941_XLAT_PORT PORTB
#define Tlc5941_XLAT_PIN PB1

#define Tlc5941_SIN_DDR DDRD
#define Tlc5941_SIN_PORT PORTD
#define Tlc5941_SIN_PIN PD1

#define Tlc5941_MODE_DDR DDRD
#define Tlc5941_MODE_PORT PORTD
#define Tlc5941_MODE_PIN PD2

#define Tlc5941_BLANK_DDR DDRD
#define Tlc5941_BLANK_PORT PORTD
#define Tlc5941_BLANK_PIN PD3

#define Tlc5941_SCLK_DDR DDRD
#define Tlc5941_SCLK_PORT PORTD
#define Tlc5941_SCLK_PIN PD4

#define Tlc5941_TIMER 2

#define Tlc5941_USART_SPI 1

#define Tlc5941_N 3

#define Tlc5941_MANUAL_DC_FUNCS 1

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Tlc5941/Tlc5941.h"

/*static const Tlc5941_gsData_t well2channel[Tlc5941_numChannels] PROGMEM = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
	24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
	36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,};*/

static const Tlc5941_gsData_t well2channel[Tlc5941_numChannels] PROGMEM = {
	15, 14, 0, 1, 31, 30, 16, 17, 47, 46, 32, 33,
	13, 12, 2, 3, 29, 28, 18, 19, 45, 44, 34, 35,
	11, 10, 4, 5, 27, 26, 20, 21, 43, 42, 36, 37,
	9, 8, 6, 7, 25, 24, 22, 23, 41, 40, 38, 39,};

static const uint8_t grayscaleCalibration[Tlc5941_numChannels]  PROGMEM = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,};
	
// If dot correction should be the same in all wells, define its value in dotCorrection
#define dotCorrection 4

// If dot correction value should be different from well to well, define individualDotCorrection 
// and set values in dotCorrectionValues
#define individualDotCorrection 1
static const uint8_t dotCorrectionValues[Tlc5941_numChannels]  PROGMEM = {
	64, 8, 64, 8, 64, 8, 64, 8, 64, 8, 64, 8,
	64, 8, 64, 8, 64, 8, 64, 8, 64, 8, 64, 8,
	64, 8, 64, 8, 64, 8, 64, 8, 64, 8, 64, 8,
	64, 8, 64, 8, 64, 8, 64, 8, 64, 8, 64, 8,};
	