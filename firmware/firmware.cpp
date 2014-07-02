/*
 * firmware.cpp
 *
 * Created: 28/02/2014 02:01:59 PM
 *  Author: Sebastian Castillo
 */ 

// Headers
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Arduino.h"
#include <SD.h>

#include "StatusLeds/StatusLeds.h"
#include "Tlc5941/Tlc5941.h"
#include "MsTimer/MsTimer.h"

// File variable
File file;

// counter for led update
uint8_t ledCounter = 0;

void UpdateLeds(void) {
	// Wait until we can modify gsData
	while(Tlc5941_gsUpdateFlag);

	// Set all LEDs to a constant value
	Tlc5941_SetAllGS(0);
	Tlc5941_SetGS(ledCounter, 4095);
	ledCounter = (ledCounter + 1) % (Tlc5941_N*16);
	
	// Set update flag
	Tlc5941_SetGSUpdateFlag();
}

void UpdateStatusLeds(void) {
	StatusLeds_Toggle(StatusLeds_LedOn);
}

int main(void) {
	
	uint8_t temp;
	uint8_t i=0;
	uint8_t fileContents[100];
	
	// Initialize TLC module
	Tlc5941_Init();
	// Set up grayscale value
	Tlc5941_SetAllDC(8);
	Tlc5941_ClockInDC();
	// Default all grayscale values to off
	Tlc5941_SetAllGS(0);
	
	// Initialize Status LEDs
	StatusLeds_Init();
	
	// Initialize ms timer
	MsTimer_Init();
	// Assign callbacks
	MsTimer_AddCallback(&UpdateLeds, 10);
	MsTimer_AddCallback(&UpdateStatusLeds, 500);
	
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
	
	// Start timer
	MsTimer_Start();
	
	// Enable Global Interrupts
	sei();

	while(1) {
	}
	return 0;
}
