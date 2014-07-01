/*
 * firmware.cpp
 *
 * Created: 28/02/2014 02:01:59 PM
 *  Author: Sebastian Castillo
 */ 

//#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "Arduino.h"
#include <SD.h>

extern "C"
{
	#include "StatusLeds/StatusLeds.h"
	#include "Tlc5941/Tlc5941.h"
};

File file;
uint8_t fileContents[100];

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
	uint8_t i=0;
	
	// Initialize pins for TLC
	Tlc5941_Init();
	
	// Initialize Status LEDs
	StatusLeds_Init();
	
	// Initialize SD card
	if (!SD.begin())
	{
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
	}
	else
	{
		file = SD.open("example.txt", FILE_READ);
		if (file) {
			while (file.available()) {
				fileContents[i] = file.read();
				i++;
			}
			file.close();
			// File contents verification
			if (fileContents[0] != 'H' || 
				fileContents[1] != 'e' || 
				fileContents[2] != 'l' || 
				fileContents[3] != 'l' || 
				fileContents[4] != 'o' || 
				fileContents[5] != ' ' || 
				fileContents[6] != 'w' || 
				fileContents[7] != 'o' || 
				fileContents[8] != 'r' || 
				fileContents[9] != 'l' || 
				fileContents[10] != 'd' || 
				fileContents[11] != '!')
			{
				StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
				StatusLeds_Set(StatusLeds_LedFin, StatusLeds_On);
			}
		}
		else
		{
			StatusLeds_Set(StatusLeds_LedFin, StatusLeds_On);
		}
	}
	
    
    // Initialize Timer 1 to generate an interruption every millisecond
	// Mode: CTC, WGM1 = 0b0100
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
