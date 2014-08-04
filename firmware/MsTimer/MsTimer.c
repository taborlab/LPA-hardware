/*
 * MsTimer.c
 *
 * Created: 01/07/2014 04:32:56 PM
 *  Author: Sebastian Castillo
 */ 

#include "MsTimer.h"
#include <avr/interrupt.h>

uint32_t MsTimer_Counter;

uint8_t MsTimer_CallbackN = 0;
uint32_t MsTimer_CallbackPeriod[MsTimer_CallbackMax];
uint32_t MsTimer_CallbackCounter[MsTimer_CallbackMax];
void (*MsTimer_Callback[MsTimer_CallbackMax])(void);

void MsTimer_Init(void) {
	// Initialize Timer 1 to generate an interruption every millisecond
	// Mode: CTC, WGM1 = 0b0100
	TCCR1B = (1 << WGM12);
	// Compare value: 0x3E7F
	OCR1A = 0x3E7F;
}

void MsTimer_Start()
{
	// Reset run time counter
	MsTimer_Counter = 0;
	// Reset callback counters
	for (uint8_t i = 0; i < MsTimer_CallbackN; i++)
	{
		MsTimer_CallbackCounter[i] = 0;
	}
	// Reset timer counter
	MsTimer_ResetCounter();
	// Enable timer and interrupt
	MsTimer_EnableTimer();
	MsTimer_EnableInt();
}

void MsTimer_Stop()
{
	MsTimer_DisableTimer();
	MsTimer_DisableInt();
}

void MsTimer_AddCallback(void (*callback)(void), uint32_t period)
{
	MsTimer_Callback[MsTimer_CallbackN] = callback;
	MsTimer_CallbackPeriod[MsTimer_CallbackN] = period;
	MsTimer_CallbackN++;
}

ISR(TIMER1_COMPA_vect) {
	// Increment global counter
	MsTimer_Counter++;
	
	// Check if we have to run callbacks and increment counter
	for (uint8_t i = 0; i < MsTimer_CallbackN; i++)
	{
		if (MsTimer_CallbackCounter[i] == 0)
		{
			(*MsTimer_Callback[i])();
		}
		MsTimer_CallbackCounter[i] = (MsTimer_CallbackCounter[i] + 1) % MsTimer_CallbackPeriod[i];
	}
}