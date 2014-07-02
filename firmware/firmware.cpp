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

// System states
#define System_stateInitializing 0
#define System_stateRunning 1
#define System_stateFinished 2
#define System_stateErrorNoSdCard 3
#define System_stateErrorNoLpf 4
#define System_stateErrorWrongLpf 5
#define System_stateErrorTimeout 6
#define System_stateErrorLpfUnavailable 7

uint8_t System_state;

#define System_SetState(state) System_state = state;
#define System_IsState(state) System_state == state
#define System_IsNotState(state) System_state != state

// Light program file
File lpf;

void UpdateLeds(void) {
	// counter for led update
	static uint8_t ledCounter = 0;

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
	switch (System_state)
	{
	case System_stateInitializing:
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_Off);
		break;
	case System_stateRunning:
		StatusLeds_Toggle(StatusLeds_LedOn);
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_Off);
		break;
	case System_stateFinished:
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_On);
		break;
	case System_stateErrorNoSdCard:
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_Off);
		break;
	case System_stateErrorNoLpf:
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_On);
		break;
	case System_stateErrorWrongLpf:
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_On);
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_Off);
		break;
	case System_stateErrorTimeout:
		StatusLeds_Toggle(StatusLeds_LedOn);
		StatusLeds_Toggle(StatusLeds_LedErr);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_Off);
		break;
	case System_stateErrorLpfUnavailable:
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_Off);
		StatusLeds_Toggle(StatusLeds_LedErr);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_Off);
		break;
	}
}

int main(void) {
	
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

	// Initialize system state
	System_SetState(System_stateInitializing);
	
	// Test if SD card is present and initialize
	if (!SD.begin())
	{
		System_SetState(System_stateErrorNoSdCard);
	}
	// Test if SD card is present
	if (System_IsState(System_stateInitializing))
	{
		lpf = SD.open("program.lpf", FILE_READ);
		if (!lpf) {
				System_SetState(System_stateErrorNoLpf);
			}
	}

	// Get headers from LPF
	// TODO
	// Verify headers from LPF
	// TODO
	// Switch to running state
	if (System_IsState(System_stateInitializing))
	{
		System_SetState(System_stateRunning);
	}
	
	// Start timer
	MsTimer_Start();
	
	// Enable Global Interrupts
	sei();

	// Do led intensity decoding as necessary
	while(1) {
		if (System_IsState(System_stateRunning))
		{
			// TODO
		}
	}
	return 0;
}
