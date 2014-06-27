/*
 * SdDriver.c
 *
 * Created: 24/06/2014 10:32:59 PM
 *  Author: Sebastian Castillo
 */ 

#include "SdDriver.h"
#include "../Spi/Spi.h"

uint8_t SdDriver_commandResponse[5];

void SdDriver_Delay(uint8_t number)
{
	for (uint8_t i = 0; i < number; i++)
	{
		// Send an idle byte
		Spi_SendByte(0xFF);
	}
}


uint8_t SdDriver_SendCommand(uint8_t command, uint8_t responseType, uint32_t argument)
{
	uint8_t responseLength;
	uint8_t responseTemp;
	uint8_t responseCount;
	
	// Send command
	// Assert SS
	Spi_SsAssert();
	// Send command header
	Spi_SendByte((command & 0x3F) | 0x40);
	// Send argument
	Spi_SendByte((argument >> 24) & 0xFF);
	Spi_SendByte((argument >> 16) & 0xFF);
	Spi_SendByte((argument >> 8) & 0xFF);
	Spi_SendByte((argument) & 0xFF);
	// Send CRC: Use constant value of 0x95, which is the valid CRC for GO_IDLE_STATE
	Spi_SendByte(0x95);
	
	// Get response length
	switch(responseType)
	{
		case SdDriver_R1:
		case SdDriver_R1B:
			responseLength = 1;
			break;
		case SdDriver_R2:
			responseLength = 2;
			break;
		case SdDriver_R3:
			responseLength = 5;
			break;
		default:
		responseLength = 0;
		break;
	}
	
	// Wait for response
	responseCount = 0;
	do 
	{
		responseTemp = Spi_ReceiveByte();
		responseCount++;
	} while (((responseTemp & 0x80) != 0 ) && responseCount < SdDriver_CMD_TIMEOUT);
	
	// Return error if we don't get a response
	if (responseCount >= SdDriver_CMD_TIMEOUT)
	{
		Spi_SsDeassert();
		return 1;
	}
	
	// Receive rest of response if necessary
	for (int8_t i = responseLength - 1; i >=0; i--)
	{
		SdDriver_commandResponse[i] = responseTemp;
		responseTemp = Spi_ReceiveByte();
	}
	
	// Manage "busy" type responses (R1B)
	// SD card will send a stream of zeros until it finishes
	// Microcontroller must wait until received value consists only of ones
	if (responseType == SdDriver_R1B)
	{
		do 
		{
			responseTemp = Spi_ReceiveByte();
		} while (responseTemp != 0xFF);
		// One more transmission/reception
		responseTemp = Spi_ReceiveByte();
	}
	
	// Return successfully
	Spi_SsDeassert();
	return 0;
}

uint8_t SdDriver_Initialize()
{
	uint8_t counter = 0;
	// Start SPI
	Spi_Init();
	// Set clock to slower than 400khz
	// Clock is 16Mhz, we divide it by 128 to get to 125khz
	Spi_SetFreqDiv(128);
	// Assert SS
	Spi_SsAssert();
	// Wait for at least 74 clock cycles
	// That's at least 10 bytes
	SdDriver_Delay(10);
	Spi_SsDeassert();
	SdDriver_Delay(2);
	
	// Instruct card to go to the idle state
	if (SdDriver_SendCommand(SdDriver_CMD_GO_IDLE_STATE, SdDriver_CMD_GO_IDLE_STATE_R, 0)) {return 1;}
	
	// Wait until card is in idle mode
	counter = 0;
	do 
	{
		counter++;
		// Next command is application-specific
		if (SdDriver_SendCommand(SdDriver_CMD_APP_CMD, SdDriver_CMD_APP_CMD_R, 0)) {return 1;}
		// Start initialization process
		if (SdDriver_SendCommand(SdDriver_ACMD_SEND_OP_COND, SdDriver_ACMD_SEND_OP_COND_R, 0)) {return 1;}
	} while ((SdDriver_commandResponse[0] & SdDriver_MSK_IDLE) == SdDriver_MSK_IDLE
				&& counter < SdDriver_IDLE_WAIT_MAX);
	
	// Check timeout
	if (counter >= SdDriver_IDLE_WAIT_MAX) {return 1;}
	
	// Request for OCR and check for 3.3V support
	if (SdDriver_SendCommand(SdDriver_CMD_READ_OCR, SdDriver_CMD_READ_OCR_R, 0)) {return 1;}
	if ((SdDriver_commandResponse[2] & SdDriver_MSK_OCR_33) != SdDriver_MSK_OCR_33) {return 1;}
		
	// Change block size
	if (SdDriver_SendCommand(SdDriver_CMD_SET_BLOCKLEN, SdDriver_CMD_SET_BLOCKLEN_R, SdDriver_BLOCKSIZE)) {return 1;}
	if ((SdDriver_commandResponse[0] & SdDriver_MSK_PARAM_ERR) == SdDriver_MSK_PARAM_ERR) {return SdDriver_commandResponse[0];}
	
	// Change SPI back to high speed
	Spi_SetFreqDiv(2);
	
	// Success
	return 0;
}

uint8_t SdDriver_ReadSingleBlock(uint32_t startBlock, uint8_t *buffer)
{
	uint16_t count;
	uint8_t temp;
	// Send read single block command
	if (SdDriver_SendCommand(SdDriver_CMD_READ_SINGLE_BLOCK, SdDriver_CMD_READ_SINGLE_BLOCK_R, startBlock << 9)) {return 1;}
	if (SdDriver_commandResponse[0] != 0x00) {return SdDriver_commandResponse[0];}
	
	// Assert Sd card
	Spi_SsAssert();
	
	// Wait for start block token
	count = 0;
	do 
	{
		temp = Spi_ReceiveByte();
		count++;
		if (count >= SdDriver_READ_BLOCK_TIMEOUT)
		{
			Spi_SsDeassert();
			return 1;
		}
		else if ((temp & SdDriver_MSK_TOK_DATAERROR) == 0)
		{
			Spi_SsDeassert();
			return temp;
		}
	} while (temp != SdDriver_TOK_READ_STARTBLOCK);
	
	// Read data
	for (uint16_t i = 0; i < SdDriver_BLOCKSIZE; i++)
	{
		buffer[i] = Spi_ReceiveByte();
	}
	
	// 16-bit CRC is ignored
	Spi_ReceiveByte();
	Spi_ReceiveByte();
	
	// Extra 8 clock pulses
	Spi_ReceiveByte();
	
	// Deassert
	Spi_SsDeassert();
	
	// Success
	return 0;
}

uint8_t SdDriver_WriteSingleBlock(uint32_t startBlock, uint8_t *buffer)
{
	uint8_t response;
	// Send write block command
	if (SdDriver_SendCommand(SdDriver_CMD_WRITE_BLOCK, SdDriver_CMD_WRITE_BLOCK_R, startBlock << 9)) {return 1;}
	if (SdDriver_commandResponse[0] != 0x00) {return SdDriver_commandResponse[0];}
	
	// Assert Sd card
	Spi_SsAssert();
	
	// Transmit start block token
	Spi_SendByte(SdDriver_TOK_WRITE_STARTBLOCK);
	// Transmit data
	for (uint16_t i = 0; i < SdDriver_BLOCKSIZE; i++)
		Spi_SendByte(buffer[i]);
	
	// Transmit dummy CRC
	Spi_SendByte(0xFF);
	Spi_SendByte(0xFF);
	
	// Get data response, return with error if data not accepted
	response = Spi_ReceiveByte();
	if ((response & SdDriver_MSK_DATA_RESPONSE) != SdDriver_MSK_DATA_ACCEPTED)
	{
		Spi_SsDeassert();
		return 1;
	}
	
	// Wait for card to complete writing
	do
	{
		response = Spi_ReceiveByte();
	} while (response != 0xFF);
	
	// One more transmission/reception
	Spi_ReceiveByte();
	
	// Deassert
	Spi_SsDeassert();
	
	// Success
	return 0;
}

uint8_t SdDriver_ReadMultipleBlock(uint32_t startBlock, uint32_t numberBlocks, uint8_t *buffer)
{
	uint16_t count;
	uint8_t temp;
	// Send read single block command
	if (SdDriver_SendCommand(SdDriver_CMD_READ_MULTIPLE_BLOCK, SdDriver_CMD_READ_MULTIPLE_BLOCK_R, startBlock << 9)) {return 1;}
	if (SdDriver_commandResponse[0] != 0x00) {return SdDriver_commandResponse[0];}
	
	// Assert Sd card
	Spi_SsAssert();
	
	for (uint8_t j = 0; j < numberBlocks; j++)
	{
		// Wait for start block token
		count = 0;
		do
		{
			temp = Spi_ReceiveByte();
			count++;
			if (count >= SdDriver_READ_BLOCK_TIMEOUT)
			{
				Spi_SsDeassert();
				return 1;
			}
			else if ((temp & SdDriver_MSK_TOK_DATAERROR) == 0)
			{
				Spi_SsDeassert();
				return temp;
			}
		} while (temp != SdDriver_TOK_READ_STARTBLOCK);
		
		// Read data
		for (uint16_t i = 0; i < SdDriver_BLOCKSIZE; i++)
		{
			*buffer = Spi_ReceiveByte();
			buffer++;
		}
		
		// 16-bit CRC is ignored
		Spi_ReceiveByte();
		Spi_ReceiveByte();
		
		// Extra 8 clock pulses
		Spi_ReceiveByte();
	}
	
	// Send stop command
	SdDriver_SendCommand(SdDriver_CMD_STOP_TRANSMISSION, SdDriver_CMD_STOP_TRANSMISSION_R, 0);
	// Extra 8 clock pulses
	Spi_ReceiveByte();
	
	// Success
	return 0;
}

uint8_t SdDriver_WriteMultipleBlock(uint32_t startBlock, uint32_t numberBlocks, uint8_t *buffer)
{
	uint8_t response;
	// Send write block command
	if (SdDriver_SendCommand(SdDriver_CMD_WRITE_MULTIPLE_BLOCK, SdDriver_CMD_WRITE_MULTIPLE_BLOCK_R, startBlock << 9)) {return 1;}
	if (SdDriver_commandResponse[0] != 0x00) {return SdDriver_commandResponse[0];}
	
	// Assert Sd card
	Spi_SsAssert();
	
	for (uint8_t j = 0; j < numberBlocks; j++)
	{
		// Transmit start block token
		Spi_SendByte(SdDriver_TOK_WRITE_STARTBLOCK_M);
		// Transmit data
		for (uint16_t i = 0; i < SdDriver_BLOCKSIZE; i++)
		{
			Spi_SendByte(*buffer);
			buffer++;
		}
	
		// Transmit dummy CRC
		Spi_SendByte(0xFF);
		Spi_SendByte(0xFF);
	
		// Get data response, return with error if data not accepted
		response = Spi_ReceiveByte();
		if ((response & SdDriver_MSK_DATA_RESPONSE) != SdDriver_MSK_DATA_ACCEPTED)
		{
			Spi_SsDeassert();
			return 1;
		}
	
		// Wait for card to complete writing
		do
		{
			response = Spi_ReceiveByte();
		} while (response != 0xFF);
	
		// One more transmission/reception
		Spi_ReceiveByte();
	}
	
	// Send stop transmission token
	Spi_SendByte(SdDriver_TOK_STOP_MULTI);
	// One more transmission/reception
	Spi_ReceiveByte();
	// Wait for card to complete writing
	do
	{
		response = Spi_ReceiveByte();
	} while (response != 0xFF);
	
	// One more transmission/reception
	Spi_ReceiveByte();
	
	// Deassert
	Spi_SsDeassert();
	
	// Success
	return 0;
}