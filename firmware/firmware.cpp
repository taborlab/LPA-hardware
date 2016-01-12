/*
 * firmware.cpp
 *
 * Created: 28/02/2014 02:01:59 PM
 *  Author: Sebastian Castillo
 */ 

// Headers
#include <stdlib.h>
#include <ctype.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

#include "Arduino.h"
#include <SD.h>

#include "StatusLeds/StatusLeds.h"
#include "Tlc5941/Tlc5941.h"
#include "MsTimer/MsTimer.h"

// Array with dot correction values
uint8_t dotCorrectionValues[Tlc5941_numChannels];

// Array with grayscale calibration values
uint8_t grayscaleCalibration[Tlc5941_numChannels];

// System states
#define System_stateInitializing 0
#define System_stateRunning 1
#define System_stateFinished 2
#define System_stateErrorNoSdCard 3
#define System_stateErrorNoSdFiles 4
#define System_stateErrorWrongSdFiles 5
#define System_stateErrorTimeout 6
#define System_stateErrorLpfUnavailable 7

volatile uint8_t System_state;

#define System_SetState(state) System_state = state;
#define System_IsState(state) System_state == state
#define System_IsNotState(state) System_state != state

// Synchronization variable
volatile int8_t dataAvailableFlag = 0;
#define Flag_Reset(f) f=0;
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
	// Release data available flag
	if (System_IsState(System_stateRunning))
	{
		Flag_Release(dataAvailableFlag);
	}

	// Set update flag for Tlc library
	if (!Flag_HasFailedRelease(dataAvailableFlag))
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
	case System_stateErrorNoSdFiles:
		StatusLeds_Set(StatusLeds_LedOn, StatusLeds_Off);
		StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
		StatusLeds_Set(StatusLeds_LedFin, StatusLeds_On);
		break;
	case System_stateErrorWrongSdFiles:
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

// This function parses a list of uint8 from a text file. It returns 1 if exactly
// n_output numbers were extracted from the file and stored in output[], and 0
// otherwise.
uint8_t parseTextFile(File file, uint8_t output[], uint32_t n_output) {
	// Index for the output array
	uint32_t output_i = 0;
	// text buffer
	const uint8_t charBuffer_length = 50;
	char charBuffer[charBuffer_length];
	uint8_t charBuffer_i = 0;
	char * charBufferPtr;
	// Temporary container for the converted integer
	uint16_t tempVal;

	// Read characters until the end of the file is found
	while (file.available()) {
		charBuffer[0] = file.read();
		// Only process if current character is not space
		if (!isspace(charBuffer[0])) {
			// If we already finished with the output array, return with error
			if (output_i == n_output) {
				return 0;
			}
			
			// Fill buffer until a space or EOF are found.
			charBuffer_i = 1;
			while(file.available() && !isspace(file.peek()) && (charBuffer_i < charBuffer_length - 1)) {
				charBuffer[charBuffer_i] = file.read();
				charBuffer_i++;
			}
			// Return with error if the buffer overflows
			if (file.available() && !isspace(file.peek())){
				return 0;
			}
			// Add end of string character
			charBuffer[charBuffer_i] = '\0';
			
			// Attempt to convert to integer, return with error if failed
			// If the end pointer set by strtoul points to the end of the string, the
			// entire string is valid.
			// We also consider the value invalid if it cannot be cast to uint8_t
			tempVal = strtoul(charBuffer, &charBufferPtr, 0);
			if (charBufferPtr != (charBuffer + charBuffer_i) || tempVal > 255) {
				return 0;
			}
			// Store in output array
			output[output_i] = tempVal;
			output_i++;
		}
	}
	
	// Check that we filled the array, return with error otherwise
	if (output_i == n_output) {
		return 1;
	}
	else {
		return 0;
	}
}

int main(void) {
	// Dot correction file
	File dcFile;
	// Calibration file
	File gcalFile;
	// Light program file
	File lpfFile;
	// Information holder for the lpf
	lpfInfo_t lpfInfo;
	// Temporary value for reading from the lpf
	uint32_t temp = 0;
	// Flag that indicates whether the first frame has been loaded
	uint8_t first_frame = 0;

	// Enable interruptions
	sei();
	
	// Initialize TLC module
	Tlc5941_Init();
	// Default all grayscale values to zero
	Tlc5941_SetAllGS(0);
	// Force upload of grayscale values
	Tlc5941_SetGSUpdateFlag();
	while(Tlc5941_gsUpdateFlag);

	// Signal that the first set of grayscale values should be used during the first iteration
	Flag_Set(dataAvailableFlag);
	
	// Initialize Status LEDs
	StatusLeds_Init();
	// Turn on all LEDs
	StatusLeds_Set(StatusLeds_LedOn, StatusLeds_On);
	StatusLeds_Set(StatusLeds_LedErr, StatusLeds_On);
	StatusLeds_Set(StatusLeds_LedFin, StatusLeds_On);
	
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

	// Process dot correction file
	if (System_IsState(System_stateInitializing)) {
		dcFile = SD.open("dc.txt", FILE_READ);
		if (dcFile) {
			// Try to parse file contents, change to error state if unsuccessful
			if (!parseTextFile(dcFile, dotCorrectionValues, Tlc5941_numChannels)) {
				System_SetState(System_stateErrorWrongSdFiles);
			}
			dcFile.close();
		}
		else {
			System_SetState(System_stateErrorNoSdFiles);
		}
	}
	// Set dot correction values
	if (System_IsState(System_stateInitializing)) {
		// Set from dotCorrectionValues array
		for (Tlc5941_channel_t i = 0; i < Tlc5941_numChannels; i++)
		{
			Tlc5941_channel_t well = pgm_read_byte(&(well2channel[i]));
			Tlc5941_SetDC(well, dotCorrectionValues[i]);
		}
	}
	else {
		// Set all to zero
		Tlc5941_SetAllDC(0);
	}
	// Push all DC values
	Tlc5941_ClockInDC();
	Tlc5941_ClockInDC();

	// Process calibration file
	if (System_IsState(System_stateInitializing)) {
		gcalFile = SD.open("gcal.txt", FILE_READ);
		if (gcalFile) {
			// Try to parse file contents, change to error state if unsuccessful
			if (!parseTextFile(gcalFile, grayscaleCalibration, Tlc5941_numChannels)) {
				System_SetState(System_stateErrorWrongSdFiles);
			}
			gcalFile.close();
		}
		else {
			System_SetState(System_stateErrorNoSdFiles);
		}
	}

	// Load LPF
	if (System_IsState(System_stateInitializing))
	{
		lpfFile = SD.open("program.lpf", FILE_READ);
		if (!lpfFile) {
			System_SetState(System_stateErrorNoSdFiles);
		}
	}

	// Get headers from LPF
	if (System_IsState(System_stateInitializing))
	{
		if (lpfFile.size() < LPF_HEADER_LENGTH)
		{
			System_SetState(System_stateErrorWrongSdFiles);
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
			System_SetState(System_stateErrorWrongSdFiles);
	
		// Check appropriate file size
		if (lpfFile.size() != (LPF_HEADER_LENGTH + lpfInfo.numberSteps*lpfInfo.numberChannels*2))
			System_SetState(System_stateErrorWrongSdFiles);
	}
	// Scale step size
	#ifdef stepSizeScaling
		lpfInfo.stepSize = (uint32_t)((double)(lpfInfo.stepSize)*stepSizeScaling);
	#endif

	// Switch to running state
	if (System_IsState(System_stateInitializing))
	{
		System_SetState(System_stateRunning);
	}
	
	// Assign callbacks to timer
	MsTimer_Stop();
	if (System_IsState(System_stateRunning))
	{
		MsTimer_AddCallback(&UpdateLeds, lpfInfo.stepSize);
		MsTimer_AddCallback(&UpdateStatusLeds, 500);
		Flag_Reset(dataAvailableFlag);
	}
	else
	{
		MsTimer_AddCallback(&UpdateStatusLeds, 500);
		// Start timer
		MsTimer_Start();
	}
	first_frame = 1;
	
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
			for (Tlc5941_channel_t i = 0; i < Tlc5941_numChannels; i++)
			{
				// We get two bytes at a time, which contains one grayscale value
				if (lpfFile.readBytes((char*)(&(temp)), 2) != 2)
				{
					System_SetState(System_stateErrorLpfUnavailable);
					break;
				}
				// Get well position and calibration
				Tlc5941_channel_t well = pgm_read_byte(&(well2channel[i]));
				uint16_t calibration = grayscaleCalibration[i];
				// Update LEDs
				// uint32_t intensity = i*10;
				// Tlc5941_SetGS(well, intensity*(calibration + 1)>>8);
				Tlc5941_SetGS(well, temp*(calibration + 1)>>8);
			}
			if (!System_IsState(System_stateRunning))
			{
				continue;
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
			// Start timer if first frame
			if (first_frame)
			{
				MsTimer_Start();
				first_frame = 0;
			}
			
		}
		else {
			// Set all grayscale to zero if first frame
			if (first_frame) {
				Tlc5941_SetAllGS(0);
				Tlc5941_SetGSUpdateFlag();
				while(Tlc5941_gsUpdateFlag);

				first_frame = 0;
			}
		}
	}
	return 0;
}
