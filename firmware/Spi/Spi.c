/*
 * Spi.c
 *
 * Created: 18/06/2014 10:10:15 PM
 *  Author: Sebastian Castillo
 */ 

#include "avr/io.h"

#include "Spi.h"

void Spi_Init()
{
	// PB2: SS, PB3: MOSI, PB4: MISO, PB5: SCK
	DDRB |=  0b00101100;
	DDRB &= ~0b00010000;
	
	// Enable SPI, Master, set clock rate fosc/2
	// Set master mode
	SPCR = (1 << MSTR);
	
	// Enable and set spi clock to fclk/128
	Spi_SetFreqDiv(128);
}

void Spi_SetFreqDiv(uint8_t freq_div)
{
	// Disable SPI
	Spi_Disable();
	// Set frequency division
	switch (freq_div)
	{
		case 2:
		SPCR &= ~(1 << SPR0);
		SPCR &= ~(1 << SPR1);
		SPSR |= (1 << SPI2X);
		break;
		
		case 64:
		SPCR &= ~(1 << SPR0);
		SPCR |= (1 << SPR1);
		SPSR &= ~(1 << SPI2X);
		break;
		
		case 128:
		default:
		SPCR |= (1 << SPR0);
		SPCR |= (1 << SPR1);
		SPSR &= ~(1 << SPI2X);
		break;
	}
	// Enable SPI
	Spi_Enable();
}

void Spi_SendByte(uint8_t input)
{
	// Send byte
	SPDR = input;
	// Wait for transmission to complete
	while (!(SPSR & (1 << SPIF)));
}

uint8_t Spi_ReceiveByte()
{
	uint8_t temp;
	// Send dummy byte
	SPDR = 0xFF;
	// Wait for transmission to complete
	while (!(SPSR & (1 << SPIF)));
	// Get received byte
	temp = SPDR;
	return temp;
}