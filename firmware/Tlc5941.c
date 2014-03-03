/*
 * Tlc5940.c
 *
 * Created: 28/02/2014 04:58:43 PM
 *  Author: Sebastian Castillo
 */ 

#include <avr/interrupt.h>

#include "Tlc5941.h"

// Variable declarations
uint8_t Tlc5941_dcData[Tlc5941_dcDataSize];
uint8_t Tlc5941_gsData[Tlc5941_gsDataSize];
volatile uint8_t Tlc5941_gsUpdateFlag;

void Tlc5941_Init(void) {
	// Define direction of pins
	setOutput(Tlc5941_GSCLK_DDR, Tlc5941_GSCLK_PIN);
	setOutput(Tlc5941_SCLK_DDR, Tlc5941_SCLK_PIN);
	setOutput(Tlc5941_MODE_DDR, Tlc5941_MODE_PIN);
	setOutput(Tlc5941_XLAT_DDR, Tlc5941_XLAT_PIN);
	setOutput(Tlc5941_BLANK_DDR, Tlc5941_BLANK_PIN);
	setOutput(Tlc5941_SIN_DDR, Tlc5941_SIN_PIN);
	
	// Set initial values of pins
	setLow(Tlc5941_GSCLK_PORT, Tlc5941_GSCLK_PIN);
	setLow(Tlc5941_SCLK_PORT, Tlc5941_SCLK_PIN);
	setHigh(Tlc5941_MODE_PORT, Tlc5941_MODE_PIN);
	setLow(Tlc5941_XLAT_PORT, Tlc5941_XLAT_PIN);
	setHigh(Tlc5941_BLANK_PORT, Tlc5941_BLANK_PIN);
	
	// SPI configuration
	// Enable SPI, Master, set clock rate fck/2
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X);
	
	// Set timer for grayscale value transmission
	// CTC with OCR0A as TOP
	TCCR0A = (1 << WGM01);
	// clk_io/1024 (From prescaler)
	TCCR0B = ((1 << CS02) | (1 << CS00));
	// Generate an interrupt every 4096 clock cycles
	OCR0A = 3;
	// Enable Timer/Counter0 Compare Match A interrupt
	TIMSK0 |= (1 << OCIE0A);
}

void Tlc5941_SetAllGS(uint16_t value) {
	// Sets all grayscale values to the same input value.
	uint8_t tmp1 = (value >> 4);
	uint8_t tmp2 = (uint8_t)(value << 4) | (tmp1 >> 4);
	Tlc5941_gsData_t i = 0;
	do {
		Tlc5941_gsData[i++] = tmp1; // bits: 11 10 09 08 07 06 05 04
		Tlc5941_gsData[i++] = tmp2; // bits: 03 02 01 00 11 10 09 08
		Tlc5941_gsData[i++] = (uint8_t)value; // bits: 07 06 05 04 03 02 01 00
	} while (i < Tlc5941_gsDataSize);
}

void Tlc5941_SetGS(Tlc5941_channel_t channel, uint16_t value) {
	// Sets the grayscale value of a particular channel
	channel = Tlc5941_numChannels - 1 - channel;
	uint16_t i = (uint16_t)channel * 3 / 2;
	switch (channel % 2) {
		case 0:
			Tlc5941_gsData[i] = (value >> 4);
			i++;
			Tlc5941_gsData[i] = (Tlc5941_gsData[i] & 0x0F) | (uint8_t)(value << 4);
			break;
		default: // case 1:
			Tlc5941_gsData[i] = (Tlc5941_gsData[i] & 0xF0) | (value >> 8);
			i++;
			Tlc5941_gsData[i] = (uint8_t)value;
			break;
	}
}

#if (Tlc5941_MANUAL_DC_FUNCS)
void Tlc5941_ClockInDC(void) {
	// Change programming mode
	setHigh(Tlc5941_MODE_PORT, Tlc5941_MODE_PIN);

	// Perform data transmission
	for (Tlc5941_dcData_t i = 0; i < Tlc5941_dcDataSize; i++) {
		// Start transmission
		SPDR = Tlc5941_dcData[i];
		// Wait for transmission complete
		while (!(SPSR & (1 << SPIF)));
	}
	pulse(Tlc5941_XLAT_PORT, Tlc5941_XLAT_PIN);
}

void Tlc5941_SetAllDC(uint8_t value) {
	// Sets all dot correction values to the same input value.
	uint8_t tmp1 = (uint8_t)(value << 2);
	uint8_t tmp2 = (uint8_t)(tmp1 << 2);
	uint8_t tmp3 = (uint8_t)(tmp2 << 2);
	tmp1 |= (value >> 4);
	tmp2 |= (value >> 2);
	tmp3 |= value;
	Tlc5941_dcData_t i = 0;
	do {
		Tlc5941_dcData[i++] = tmp1; // bits: 05 04 03 02 01 00 05 04
		Tlc5941_dcData[i++] = tmp2; // bits: 03 02 01 00 05 04 03 02
		Tlc5941_dcData[i++] = tmp3; // bits: 01 00 05 04 03 02 01 00
	} while (i < Tlc5941_dcDataSize);
}

void Tlc5941_SetDC(Tlc5941_channel_t channel, uint8_t value) {
	// Sets the dot correction value of a particular channel
	channel = Tlc5941_numChannels - 1 - channel;
	uint16_t i = (uint16_t)channel * 3 / 4;
	switch (channel % 4) {
		case 0:
			Tlc5941_dcData[i] = (Tlc5941_dcData[i] & 0x03) | (uint8_t)(value << 2);
			break;
		case 1:
			Tlc5941_dcData[i] = (Tlc5941_dcData[i] & 0xFC) | (value >> 4);
			i++;
			Tlc5941_dcData[i] = (Tlc5941_dcData[i] & 0x0F) | (uint8_t)(value << 4);
			break;
		case 2:
			Tlc5941_dcData[i] = (Tlc5941_dcData[i] & 0xF0) | (value >> 2);
			i++;
			Tlc5941_dcData[i] = (Tlc5941_dcData[i] & 0x3F) | (uint8_t)(value << 6);
			break;
		default: // case 3:
			Tlc5941_dcData[i] = (Tlc5941_dcData[i] & 0xC0) | (value);
			break;
	}
}
#endif // #if (Tlc5941_MANUAL_DC_FUNCS)

ISR(TIMER0_COMPA_vect) {
	static uint8_t xlatNeedsPulse = 0;
	
	// Make the TLC load new values
	setHigh(Tlc5941_BLANK_PORT, Tlc5941_BLANK_PIN);
	
	if (outputState(Tlc5941_MODE_PORT, Tlc5941_MODE_PIN)) {
		setLow(Tlc5941_MODE_PORT, Tlc5941_MODE_PIN);
		if (xlatNeedsPulse) {
			pulse(Tlc5941_XLAT_PORT, Tlc5941_XLAT_PIN);
			xlatNeedsPulse = 0;
		}
		pulse(Tlc5941_SCLK_PORT, Tlc5941_SCLK_PIN);
		} else if (xlatNeedsPulse) {
		pulse(Tlc5941_XLAT_PORT, Tlc5941_XLAT_PIN);
		xlatNeedsPulse = 0;
	}
	
	setLow(Tlc5941_BLANK_PORT, Tlc5941_BLANK_PIN);
	
	// Send grayscale data only if gsUpdateFlag is set
	if (Tlc5941_gsUpdateFlag) {
		// Below this we have 4096 cycles to shift in the data for the next cycle
		for (Tlc5941_gsData_t i = 0; i < Tlc5941_gsDataSize; i++) {
			SPDR = Tlc5941_gsData[i];
			while (!(SPSR & (1 << SPIF)));
		}
		xlatNeedsPulse = 1;
		Tlc5941_gsUpdateFlag = 0;
	}
}