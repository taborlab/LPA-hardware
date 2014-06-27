/*
 * Spi.h
 *
 * Created: 18/06/2014 10:09:59 PM
 *  Author: Sebastian Castillo
 */ 


#ifndef SPI_H_
#define SPI_H_

// Macros to enable or disable the SPI module
#define Spi_Disable() SPCR &= ~(1 << SPE)
#define Spi_Enable() SPCR |=  (1 << SPE)

// Function to initialize the module
void Spi_Init(void);
// Function to change the frequency divider in the clock module
void Spi_SetFreqDiv(uint8_t freq_div);
// Functions to assert or deassert the selection pin
#define Spi_SsAssert()   PORTB &= ~0b00000100;
#define Spi_SsDeassert() PORTB |=  0b00000100;
// Function to send a data byte
void Spi_SendByte(uint8_t input);
// Function to receive
uint8_t Spi_ReceiveByte(void);


#endif /* SPI_H_ */