/*
 * firmware.c
 *
 * Created: 28/02/2014 02:01:59 PM
 *  Author: Sebastian Castillo
 */ 

//#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "StatusLeds/StatusLeds.h"
#include "Tlc5941/Tlc5941.h"
#include "Spi/Spi.h"
#include "SdDriver/SdDriver.h"

//uint8_t inputBuffer[SdDriver_BLOCKSIZE];
//uint8_t outputBuffer[SdDriver_BLOCKSIZE];
uint8_t buffer[3*SdDriver_BLOCKSIZE];

// Initialize variables for demo
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


ISR(TIMER1_COMPA_vect) {
	static uint16_t count = 0; // milliseconds
	static uint16_t count2 = 0; // milliseconds
	
	Tlc5941_channel_t i = 0; // Tlc channel
	
	if (count == 0)
	{
		// Wait until we can modify gsData
		while(Tlc5941_gsUpdateFlag);
		
		/*// "Snake" test
		// Set all grayscale values to zero
		Tlc5941_SetAllGS(0);
		// Update appropriate grayscale values
		for (i = 0; i < 10; i++)
		{
			Tlc5941_SetGS((i + shift) % Tlc5941_numChannels, wave[wave_index][i]);
		}
		// Update indices
		wave_index = wave_index + 1;
		if (wave_index==10)
		{
			wave_index = 0;
			shift = (shift + 1) % Tlc5941_numChannels;
		}*/
		
		// Ramp test
		Tlc5941_SetAllGS(((uint16_t)wave_index)*16);
		wave_index = (wave_index + 1) % 255;
		
		// Fixed tests
		//Tlc5941_SetAllGS(0b101010101010);
		//Tlc5941_SetAllGS(4095);
		//Tlc5941_SetAllGS(2048);
		
		// Set update flag
		Tlc5941_SetGSUpdateFlag();
	}
	
	if (count2 == 0)
	{
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_On);
	}
	else if (count2 == 500)
	{
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_Off);
	}
	
	// Increment counter
	count = (count + 1) % 10;
	count2 = (count2 + 1) % 1000;
}

int main(void) {
	
	uint8_t temp;
	
	// Initialize pins for TLC
	Tlc5941_Init();
	
	// Initialize Status LEDs
	StatusLeds_Init();
	
	// Initialize SD card controller
	if (SdDriver_Initialize())
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
	else
	{
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_Off);
		// Test reading a block
		//SdDriver_ReadSingleBlock(640, inputBuffer);
		//SdDriver_ReadSingleBlock(641, outputBuffer);
		
		// Test reading multiple blocks
		//SdDriver_ReadMultipleBlock(640, 3, buffer);
		
		// Test reading and writing a block
		//SdDriver_ReadSingleBlock(640, inputBuffer);
		//inputBuffer[0] = 'H';
		//SdDriver_WriteSingleBlock(640, inputBuffer);
		
		// Test reading and writing multiple blocks
		SdDriver_ReadMultipleBlock(639, 2, buffer);
		buffer[512] = 'H';
		buffer[513] = 'e';
		buffer[514] = 'l';
		SdDriver_WriteMultipleBlock(639, 2, buffer);
	}
    
    // Initialize Timer 5 to generate an interruption every millisecond
	// Mode: CTC, WGM5 = 0b0100
	TCCR1B = (1 << WGM12);
	// Clock: no prescaling
	TCCR1B |= (1 << CS10);
	// Compare value: 0x3E7F
	OCR1A = 0x3E7F;
	// Enable compare interrupt
	TIMSK1 = (1 << OCIE1A);
	
	// The following two lines are optional
	Tlc5941_SetAllDC(16);
	Tlc5941_ClockInDC();
	
	// Default all channels to off
	Tlc5941_SetAllGS(0);
	
	// Enable Global Interrupts
	sei();

	while(1) {
	}
	return 0;
}
