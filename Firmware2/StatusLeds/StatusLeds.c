/*
 * StatusLeds.c
 *
 * Created: 18/06/2014 03:29:27 PM
 *  Author: Sebastian Castillo
 */ 

#include <avr/interrupt.h>

#include "StatusLeds.h"

void StatusLeds_Init(void)
{
	// Set pins as outputs
	DDRC |= 0x38;
}

void StatusLeds_Set(uint8_t led, uint8_t status)
{
	if (status)
	PORTC |= 1 << led;
	else
	PORTC &= ~( 1 << led);
}

void StatusLeds_Toggle(uint8_t led)
{
	PINC = (1 << led);
}