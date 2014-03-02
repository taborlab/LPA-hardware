/*
 * firmware.c
 *
 * Created: 28/02/2014 02:01:59 PM
 *  Author: Sebastian Castillo
 */ 

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define GSCLK_DDR DDRB
#define GSCLK_PORT PORTB
#define GSCLK_PIN PB0

#define SIN_DDR DDRB
#define SIN_PORT PORTB
#define SIN_PIN PB3

#define SCLK_DDR DDRB
#define SCLK_PORT PORTB
#define SCLK_PIN PB5

#define BLANK_DDR DDRB
#define BLANK_PORT PORTB
#define BLANK_PIN PB2

#define VPRG_DDR DDRD
#define VPRG_PORT PORTD
#define VPRG_PIN PD4

#define XLAT_DDR DDRB
#define XLAT_PORT PORTB
#define XLAT_PIN PB1

#ifndef TLC5940_N
#define TLC5940_N 1
#endif

#define setOutput(ddr, pin) ((ddr) |= (1 << (pin)))
#define setLow(port, pin) ((port) &= ~(1 << (pin)))
#define setHigh(port, pin) ((port) |= (1 << (pin)))
#define pulse(port, pin) do { \
	setHigh((port), (pin)); \
	setLow((port), (pin)); \
} while (0)
#define outputState(port, pin) ((port) & (1 << (pin)))

// Calculate appropriate data type for size of the dcData array
#if (12 * TLC5940_N > 255)
	#define dcData_t uint16_t
#else
	#define dcData_t uint8_t
#endif
#define dcDataSize ((dcData_t)12 * TLC5940_N)
// Calculate appropriate data type for size of the gsData array
#if (24 * TLC5940_N > 255)
	#define gsData_t uint16_t
#else
	#define gsData_t uint8_t
#endif
#define gsDataSize ((gsData_t)24 * TLC5940_N)
// Calculate appropriate data type for number of channels
#if (16 * TLC5940_N > 255)
	#define channel_t uint16_t
#else
	#define channel_t uint8_t
#endif
#define numChannels ((channel_t)16 * TLC5940_N)

// Data structures that store the dot-correction and grayscale values
uint8_t dcData[dcDataSize];
uint8_t gsData[gsDataSize];

// Flag that indicates if grayscale values should be sent in the next ISR excecution.
volatile uint8_t gsUpdateFlag;

void TLC5940_Init(void);
void TLC5940_ClockInDC(void);
void TLC5940_SetAllGS(uint16_t value);
void TLC5940_SetAllDC(uint8_t value);
void TLC5940_SetGS(channel_t channel, uint16_t value);
void TLC5940_SetDC(channel_t channel, uint8_t value);

static inline void TLC5940_SetGSUpdateFlag(void) {
	__asm__ volatile ("" ::: "memory");
	gsUpdateFlag = 1;
}

void TLC5940_Init(void) {
	// Define direction of pins
	setOutput(GSCLK_DDR, GSCLK_PIN);
	setOutput(SCLK_DDR, SCLK_PIN);
	setOutput(VPRG_DDR, VPRG_PIN);
	setOutput(XLAT_DDR, XLAT_PIN);
	setOutput(BLANK_DDR, BLANK_PIN);
	setOutput(SIN_DDR, SIN_PIN);
	
	// Set initial values of pins
	setLow(GSCLK_PORT, GSCLK_PIN);
	setLow(SCLK_PORT, SCLK_PIN);
	setHigh(VPRG_PORT, VPRG_PIN);
	setLow(XLAT_PORT, XLAT_PIN);
	setHigh(BLANK_PORT, BLANK_PIN);
	
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

void TLC5940_ClockInDC(void) {
	// Change programming mode
	setHigh(VPRG_PORT, VPRG_PIN);

	// Perform data transmission
	for (dcData_t i = 0; i < dcDataSize; i++) {
		// Start transmission
		SPDR = dcData[i];
		// Wait for transmission complete
		while (!(SPSR & (1 << SPIF)));
	}
	pulse(XLAT_PORT, XLAT_PIN);
}

void TLC5940_SetAllGS(uint16_t value) {
	// Sets all grayscale values to the same input value.
	uint8_t tmp1 = (value >> 4);
	uint8_t tmp2 = (uint8_t)(value << 4) | (tmp1 >> 4);
	gsData_t i = 0;
	do {
		gsData[i++] = tmp1; // bits: 11 10 09 08 07 06 05 04
		gsData[i++] = tmp2; // bits: 03 02 01 00 11 10 09 08
		gsData[i++] = (uint8_t)value; // bits: 07 06 05 04 03 02 01 00
	} while (i < gsDataSize);
}

void TLC5940_SetAllDC(uint8_t value) {
	// Sets all dot correction values to the same input value.
	uint8_t tmp1 = (uint8_t)(value << 2);
	uint8_t tmp2 = (uint8_t)(tmp1 << 2);
	uint8_t tmp3 = (uint8_t)(tmp2 << 2);
	tmp1 |= (value >> 4);
	tmp2 |= (value >> 2);
	tmp3 |= value;
	dcData_t i = 0;
	do {
		dcData[i++] = tmp1; // bits: 05 04 03 02 01 00 05 04
		dcData[i++] = tmp2; // bits: 03 02 01 00 05 04 03 02
		dcData[i++] = tmp3; // bits: 01 00 05 04 03 02 01 00
	} while (i < dcDataSize);
}

void TLC5940_SetGS(channel_t channel, uint16_t value) {
	// Sets the grayscale value of a particular channel
	channel = numChannels - 1 - channel;
	uint16_t i = (uint16_t)channel * 3 / 2;
	switch (channel % 2) {
		case 0:
		gsData[i] = (value >> 4);
		i++;
		gsData[i] = (gsData[i] & 0x0F) | (uint8_t)(value << 4);
		break;
		default: // case 1:
		gsData[i] = (gsData[i] & 0xF0) | (value >> 8);
		i++;
		gsData[i] = (uint8_t)value;
		break;
	}
}

void TLC5940_SetDC(channel_t channel, uint8_t value) {
	// Sets the dot correction value of a particular channel
	channel = numChannels - 1 - channel;
	uint16_t i = (uint16_t)channel * 3 / 4;
	switch (channel % 4) {
		case 0:
		dcData[i] = (dcData[i] & 0x03) | (uint8_t)(value << 2);
		break;
		case 1:
		dcData[i] = (dcData[i] & 0xFC) | (value >> 4);
		i++;
		dcData[i] = (dcData[i] & 0x0F) | (uint8_t)(value << 4);
		break;
		case 2:
		dcData[i] = (dcData[i] & 0xF0) | (value >> 2);
		i++;
		dcData[i] = (dcData[i] & 0x3F) | (uint8_t)(value << 6);
		break;
		default: // case 3:
		dcData[i] = (dcData[i] & 0xC0) | (value);
		break;
	}
}

ISR(TIMER0_COMPA_vect) {
	static uint8_t xlatNeedsPulse = 0;
	
	// Make the TLC load new values 
	setHigh(BLANK_PORT, BLANK_PIN);
	
	if (outputState(VPRG_PORT, VPRG_PIN)) {
		setLow(VPRG_PORT, VPRG_PIN);
		if (xlatNeedsPulse) {
			pulse(XLAT_PORT, XLAT_PIN);
			xlatNeedsPulse = 0;
		}
		pulse(SCLK_PORT, SCLK_PIN);
		} else if (xlatNeedsPulse) {
		pulse(XLAT_PORT, XLAT_PIN);
		xlatNeedsPulse = 0;
	}
	
	setLow(BLANK_PORT, BLANK_PIN);
	
	// Send grayscale data only if gsUpdateFlag is set
	if (gsUpdateFlag) {
		// Below this we have 4096 cycles to shift in the data for the next cycle
		for (gsData_t i = 0; i < gsDataSize; i++) {
			SPDR = gsData[i];
			while (!(SPSR & (1 << SPIF)));
		}
		xlatNeedsPulse = 1;
		gsUpdateFlag = 0;
	}
}

int main(void) {
	// Initialize variable for demo
	uint16_t wave[10][10] = 
	{{7, 121, 858, 2770, 4095, 2770, 858, 121, 7, 0},
	 {5, 95, 731, 2552, 4079, 2984, 999, 153, 10, 0},
	 {4, 75, 618, 2333, 4031, 3189, 1155, 191, 14, 0},
	 {2, 58, 518, 2116, 3953, 3381, 1324, 237, 19, 0},
	 {2, 44, 431, 1904, 3846, 3557, 1506, 292, 25, 1},
	 {1, 34, 356, 1700, 3714, 3714, 1700, 356, 34, 1},
	 {1, 25, 292, 1506, 3557, 3846, 1904, 431, 44, 2},
	 {0, 19, 237, 1324, 3381, 3953, 2116, 518, 58, 2},
	 {0, 14, 191, 1155, 3189, 4031, 2333, 618, 75, 4},
	 {0, 10, 153, 999, 2984, 4079, 2552, 731, 95, 5}};
	uint8_t shift = 0;
	uint8_t wave_index=0;
	
	// Initialize pins for TLC
	TLC5940_Init();
	
	// The following two lines are optional
	TLC5940_SetAllDC(63);
	TLC5940_ClockInDC();
	
	// Default all channels to off
	TLC5940_SetAllGS(0);
	
	// Enable Global Interrupts
	sei();

	channel_t i = 0;
	while(1) {
		while(gsUpdateFlag); // wait until we can modify gsData
		TLC5940_SetAllGS(0);
		/*TLC5940_SetGS((i + numChannels - 2) % numChannels, 100);
		TLC5940_SetGS((i + numChannels - 1) % numChannels, 750);
		TLC5940_SetGS(i, 4095);
		TLC5940_SetGS((i + 1) % numChannels, 750);
		TLC5940_SetGS((i + 2) % numChannels, 100);*/
		for (i = 0; i < 10; i++)
		{
			TLC5940_SetGS((i + shift) % numChannels, wave[wave_index][i]);
		}
		wave_index = wave_index + 1;
		if (wave_index==10)
		{
			wave_index = 0;
			shift = (shift + 1) % numChannels;
		}
		TLC5940_SetGSUpdateFlag();
		_delay_ms(10);
		i = (i + 1) % numChannels;
	}
	return 0;
}
