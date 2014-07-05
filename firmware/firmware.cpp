/*
 * firmware.cpp
 *
 * Created: 28/02/2014 02:01:59 PM
 *  Author: Sebastian Castillo
 */ 

// Headers
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "Arduino.h"
#include <SD.h>

#include "StatusLeds/StatusLeds.h"
#include "Tlc5941/Tlc5941.h"
#include "MsTimer/MsTimer.h"

#define OPERATION_MODE_CALIBRATION 1
#define OPERATION_MODE_NORMAL 2
#define OPERATION_MODE OPERATION_MODE_CALIBRATION

// System states
#define System_stateInitializing 0
#define System_stateRunning 1
#define System_stateFinished 2
#define System_stateErrorNoSdCard 3
#define System_stateErrorNoLpf 4
#define System_stateErrorWrongLpf 5
#define System_stateErrorTimeout 6
#define System_stateErrorLpfUnavailable 7

volatile uint8_t System_state;

#define System_SetState(state) System_state = state;
#define System_IsState(state) System_state == state
#define System_IsNotState(state) System_state != state

// Synchronization variable
volatile int8_t dataAvailableFlag = 0;
#define Flag_Set(f) f++
#define Flag_Release(f) f--
#define Flag_Wait(f) while(f)
#define Flag_IsSet(f) (f>0)
#define Flag_HasFailedRelease(f) (f<0)

typedef struct
{
	uint32_t fileVersion;
	uint32_t numberChannels;
	uint32_t stepSize;
	uint32_t numberSteps;
	uint32_t counterStep;
} lpfInfo_t;

#define LPF_HEADER_LENGTH 32

// Periodic functions
void UpdateLeds(void) {
	#if (OPERATION_MODE == OPERATION_MODE_CALIBRATION)
		while(Tlc5941_gsUpdateFlag);
		for (Tlc5941_gsData_t i = 0; i < Tlc5941_numChannels; i++)
			if (i%2 == 0)
				Tlc5941_SetGS(i, 100);
		Tlc5941_SetGSUpdateFlag();
	#elif (OPERATION_MODE == OPERATION_MODE_NORMAL)
		// Release data available flag
		if (System_IsState(System_stateRunning))
		{
			Flag_Release(dataAvailableFlag);
		}

		// Set update flag for Tlc library
		if (!Flag_HasFailedRelease(dataAvailableFlag))
			Tlc5941_SetGSUpdateFlag();
	#endif
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

// This function is required for proper operation of the arduino libraries.
// It uses the same settings for timer 0 as init() in wiring.c
void timer0_init()
{
	#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
	#endif
	#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
	#endif
	
	// Timer mode: fast PWM
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);

	// set timer 0 prescale factor to 64
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);

	// enable timer 0 overflow interrupt
	sbi(TIMSK0, TOIE0);
}

int main(void) {
	#if (OPERATION_MODE == OPERATION_MODE_NORMAL)
		// Light program file
		File lpfFile;
		// Information holder for the lpf
		lpfInfo_t lpfInfo;
	
		uint32_t temp = 0;

		// Enable interruptions
		sei();
	
		// Initialize TLC module
		Tlc5941_Init();
		// Set up grayscale value
		Tlc5941_SetAllDC(8);
		Tlc5941_ClockInDC();
		// Default all grayscale values to off
		Tlc5941_SetAllGS(0);
		// Force upload of grayscale values
		Tlc5941_SetGSUpdateFlag();
		while(Tlc5941_gsUpdateFlag);

		// Signal that the first set of grayscale values should be used during the first iteration
		Flag_Set(dataAvailableFlag);
	
		// Initialize Status LEDs
		StatusLeds_Init();
	
		// Initialize ms timer
		MsTimer_Init();

		// Initialize timer 0 before using the SD card library
		timer0_init();
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
			lpfFile = SD.open("program.lpf", FILE_READ);
			if (!lpfFile) {
					System_SetState(System_stateErrorNoLpf);
				}
		}

		// Get headers from LPF
		if (System_IsState(System_stateInitializing))
		{
			if (lpfFile.size() < LPF_HEADER_LENGTH)
			{
				System_SetState(System_stateErrorWrongLpf);
			}
			else
			{
				lpfFile.readBytes((char*)(&(lpfInfo.fileVersion)), 4);
				lpfFile.readBytes((char*)(&(lpfInfo.numberChannels)), 4);
				lpfFile.readBytes((char*)(&(lpfInfo.stepSize)), 4);
				lpfFile.readBytes((char*)(&(lpfInfo.numberSteps)), 4);
				lpfInfo.counterStep = 0;
			}
		}
		// Verify headers from LPF
		if (System_IsState(System_stateInitializing))
		{
			// Check appropriate number of channels
			if (lpfInfo.numberChannels != Tlc5941_numChannels)
				System_SetState(System_stateErrorWrongLpf);
	
			// Check appropriate file size
			if (lpfFile.size() != (LPF_HEADER_LENGTH + lpfInfo.numberSteps*lpfInfo.numberChannels*3/2))
				System_SetState(System_stateErrorWrongLpf);
		}
		// Switch to running state
		if (System_IsState(System_stateInitializing))
		{
			System_SetState(System_stateRunning);
		}
	
		// Assign callbacks to timer
		if (System_IsState(System_stateInitializing))
			MsTimer_AddCallback(&UpdateLeds, lpfInfo.stepSize);
		MsTimer_AddCallback(&UpdateStatusLeds, 500);
		// Start timer
		MsTimer_Start();

		// Do led intensity decoding as necessary
		lpfFile.seek(LPF_HEADER_LENGTH);
		while(1) {
			if (System_IsState(System_stateRunning))
			{
				// Wait until data has been consumed
				while(Flag_IsSet(dataAvailableFlag));

				// Wait until TLC library is done transmitting
				while(Tlc5941_gsUpdateFlag);

				// Check if finished
				if (lpfInfo.counterStep == lpfInfo.numberSteps)
				{
					System_SetState(System_stateFinished);
					continue;
				}
				// Read data from LPF
				for (Tlc5941_gsData_t i = 0; i < Tlc5941_numChannels/2; i++)
				{
					// We get three bytes at a time, which contains two grayscale values
					if (lpfFile.readBytes((char*)(&(temp)), 3) != 3)
					{
						System_SetState(System_stateErrorLpfUnavailable);
						continue;
					}
					// Update LEDs
					// No position decoding
					Tlc5941_SetGS(i*2, (uint16_t)(temp & 0xFFF));
					Tlc5941_SetGS(i*2 + 1, (uint16_t)((temp>>12) & 0xFFF));
					// With position decoding
					/*Tlc5941_SetGS(well2channel[i*2], (uint16_t)(temp & 0xFFF));
					Tlc5941_SetGS(well2channel[i*2 + 1], (uint16_t)((temp>>12) & 0xFFF));*/
					/*// With position decoding and calibration scaling
					Tlc5941_SetGS(well2channel[i*2], ((uint16_t)(temp & 0xFFF)*grayscaleCalibration[i*2])/255);
					Tlc5941_SetGS(well2channel[i*2 + 1], ((uint16_t)((temp>>12) & 0xFFF)*grayscaleCalibration[i*2 + 1])/255);*/
				}
				// Increment time counter
				lpfInfo.counterStep++;

				// Check if last data access was met
				// This should be run as an atomic block
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					if (Flag_HasFailedRelease(dataAvailableFlag))
					{
						System_SetState(System_stateErrorTimeout);
					}
					else
					{
						Flag_Set(dataAvailableFlag);
					}
				}
			}
		}
		return 0;
	#elif (OPERATION_MODE == OPERATION_MODE_CALIBRATION)
		// Enable interruptions
		sei();
	
		// Initialize TLC module
		Tlc5941_Init();
		// Set up grayscale value
		Tlc5941_SetAllDC(8);
		Tlc5941_ClockInDC();
		// Default all grayscale values to off
		Tlc5941_SetAllGS(0);
		// Force upload of grayscale values
		Tlc5941_SetGSUpdateFlag();
		while(Tlc5941_gsUpdateFlag);
	
		// Initialize Status LEDs
		StatusLeds_Init();
	
		// Initialize ms timer
		MsTimer_Init();
		// Add callbacks
		MsTimer_AddCallback(&UpdateLeds, 100);
		MsTimer_AddCallback(&UpdateStatusLeds, 500);
		// Start timer
		MsTimer_Start();
	
		while (1)
		{
		}
		return 0;
	#endif
}
