/*
 * firmware.cpp
 *
 * Created: 26/02/2014 11:18:06 PM
 *  Author: Sebastian Castillo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "Tlc5940/Tlc5940.h"

int main(void)
{
	/************ Configuration *************/
	// Initialize TLCs
	Tlc.init();
	Tlc.clear();
	
    while(1)
    {
		for (int i = 0; i < 8; i++)
		{
			Tlc.set(i, 500);
		}
		Tlc.update();
    }
}