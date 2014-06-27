/*
 * SdDriver.h
 *
 * Created: 24/06/2014 10:33:10 PM
 *  Author: Sebastian Castillo
 */ 


#ifndef SDDRIVER_H_
#define SDDRIVER_H_

#include <avr/io.h>

/**************************** CONSTANTS ****************************/

// Timeout constants
// Timeout for command response
#define SdDriver_CMD_TIMEOUT 100
// Number of times to retry the probe cycle during initialization
#define SdDriver_INIT_TRY 50
// Number of tries for the card to go to the idle state during initialization
#define SdDriver_IDLE_WAIT_MAX 100
// Timeout for data from read block command
#define SdDriver_READ_BLOCK_TIMEOUT 30000

// Block size for read/write operations
#define SdDriver_BLOCKSIZE 512

// Error masks
#define SdDriver_MSK_IDLE					0x01
#define SdDriver_MSK_ERASE_RST				0x02
#define SdDriver_MSK_ILL_CMD				0x04
#define SdDriver_MSK_CRC_ERR				0x08
#define SdDriver_MSK_ERASE_SEQ_ERR			0x10
#define SdDriver_MSK_ADDR_ERR				0x20
#define SdDriver_MSK_PARAM_ERR				0x40

// Tokens
#define SdDriver_TOK_READ_STARTBLOCK		0xFE
#define SdDriver_TOK_WRITE_STARTBLOCK		0xFE
#define SdDriver_TOK_READ_STARTBLOCK_M		0xFE
#define SdDriver_TOK_WRITE_STARTBLOCK_M		0xFC
#define SdDriver_TOK_STOP_MULTI				0xFD

// Error token
#define SdDriver_MSK_TOK_DATAERROR			0xE0
// Individual bytes
#define SdDriver_MSK_TOK_ERROR				0x01
#define SdDriver_MSK_TOK_CC_ERROR			0x02
#define SdDriver_MSK_TOK_ECC_FAILED			0x04
#define SdDriver_MSK_TOK_CC_OUTOFRANGE		0x08
#define SdDriver_MSK_TOK_CC_LOCKED			0x10

// Data response
#define SdDriver_MSK_DATA_RESPONSE			0x1F
#define SdDriver_MSK_DATA_ACCEPTED			(0x04 | 0x01)
#define SdDriver_MSK_DATA_CRC_ERROR			(0x0A | 0x01)
#define SdDriver_MSK_DATA_WRITE_ERROR		(0x0C | 0x01)

// Mask that extracts bits in OCR corresponding to voltage from 3.2V to 3.4V
#define SdDriver_MSK_OCR_33					0x30

// Response types
#define SdDriver_R1 1
#define SdDriver_R1B 2
#define SdDriver_R2 3
#define SdDriver_R3 4

// Basic commands
// GO_IDLE_STATE: Resets the SD card
#define SdDriver_CMD_GO_IDLE_STATE 0
#define SdDriver_CMD_GO_IDLE_STATE_R SdDriver_R1
// SEND_OP_COND: Activates the card's initialization process
#define SdDriver_CMD_SEND_OP_COND 1
#define SdDriver_CMD_SEND_OP_COND_R SdDriver_R1
// SEND_CSD: Asks the selected card to send its card-specific data (CSD)
#define SdDriver_CMD_SEND_CSD 9
#define SdDriver_CMD_SEND_CSD_R SdDriver_R1
// SEND_CID: Asks the selected card to send its card identification (CID)
#define SdDriver_CMD_SEND_CID 10
#define SdDriver_CMD_SEND_CID_R SdDriver_R1
// STOP_TRANSMISSION: Forces the card to stop transmission during a multiple block read operation
#define SdDriver_CMD_STOP_TRANSMISSION 12
#define SdDriver_CMD_STOP_TRANSMISSION_R SdDriver_R1B
// SEND_STATUS: Asks the selected card to send its status register
#define SdDriver_CMD_SEND_STATUS 13
#define SdDriver_CMD_SEND_STATUS_R SdDriver_R2

// Block read commands
// SET_BLOCKLEN: Selects a block length (in bytes) for all following block commands (read & write)
#define SdDriver_CMD_SET_BLOCKLEN 16
#define SdDriver_CMD_SET_BLOCKLEN_R SdDriver_R1
// READ_SINGLE_BLOCK: Reads a block of the size selected by the SET_BLOCKLEN command
#define SdDriver_CMD_READ_SINGLE_BLOCK 17
#define SdDriver_CMD_READ_SINGLE_BLOCK_R SdDriver_R1
// READ_MULTIPLE_BLOCK: Continuously transfers data blocks from card to host until interrupted by a STOP_TRANSMISSION command
#define SdDriver_CMD_READ_MULTIPLE_BLOCK 18
#define SdDriver_CMD_READ_MULTIPLE_BLOCK_R SdDriver_R1

// Block write commands
// WRITE_BLOCK: Writes a block of the size selected by the SET_BLOCKLEN command
#define SdDriver_CMD_WRITE_BLOCK 24
#define SdDriver_CMD_WRITE_BLOCK_R SdDriver_R1
// WRITE_MULTIPLE_BLOCK: Continuously writes blocks of data until a stop transmission token is sent
#define SdDriver_CMD_WRITE_MULTIPLE_BLOCK 25
#define SdDriver_CMD_WRITE_MULTIPLE_BLOCK_R SdDriver_R1
// PROGRAM_CSD: Programming of the programmable bits of the CSD
#define SdDriver_CMD_PROGRAM_CSD 27
#define SdDriver_CMD_PROGRAM_CSD_R SdDriver_R1

// Write protection commands
// SET_WRITE_PROT: If the card has protection features, this command sets the write protection bit of the addressed group
#define SdDriver_CMD_SET_WRITE_PROT 28
#define SdDriver_CMD_SET_WRITE_PROT_R SdDriver_R1B
// CLR_WRITE_PROT: If the card has protection features, this command clears the write protection bit of the addressed group
#define SdDriver_CMD_CLR_WRITE_PROT 29
#define SdDriver_CMD_CLR_WRITE_PROT_R SdDriver_R1B
// SEND_WRITE_PROT: If the card has write protection features, this command asks the card to send the status of the write protection bits
#define SdDriver_CMD_SEND_WRITE_PROT 30
#define SdDriver_CMD_SEND_WRITE_PROT_R SdDriver_R1

// Erase commands
// ERASE_WR_BLK_START_ADDR: Sets the address of the first write block to be erased
#define SdDriver_CMD_ERASE_WR_BLOCK_START_ADDR 32
#define SdDriver_CMD_ERASE_WR_BLOCK_START_ADDR_R SdDriver_R1
// ERASE_WR_BLK_END_ADDR: Sets the address of the last write block in a continuous range to be erased
#define SdDriver_CMD_ERASE_WR_BLOCK_END_ADDR 33
#define SdDriver_CMD_ERASE_WR_BLOCK_END_ADDR_R SdDriver_R1
// ERASE: Erases all previously selected write blocks
#define SdDriver_CMD_ERASE 38
#define SdDriver_CMD_ERASE_R R1B

// Application-specific-related commands
// APP_CMD: Notifies the card that the next command is an application specific command rather than a standard command.
#define SdDriver_CMD_APP_CMD 55
#define SdDriver_CMD_APP_CMD_R SdDriver_R1
// GEN_CMD: Used either to transfer a Data Block to the card or to get a Data Block from the card for general purpose/application specific commands
#define SdDriver_CMD_GEN_CMD 56
#define SdDriver_CMD_GEN_CMD_R SdDriver_R1
// READ_OCR: Reads the OCR register of a card
#define SdDriver_CMD_READ_OCR 58
#define SdDriver_CMD_READ_OCR_R SdDriver_R3
// CRC_ON_OFF: Turns the CRC option on or off
#define SdDriver_CMD_CRC_ON_OFF 59
#define SdDriver_CMD_CRC_ON_OFF_R SdDriver_R1

// Application-specific commands
// SD_STATUS
#define SdDriver_ACMD_SD_STATUS 13
#define SdDriver_ACMD_SD_STATUS_R SdDriver_R2
// SEND_NUM_WR_BLOCKS
#define SdDriver_ACMD_SEND_NUM_WR_BLOCKS 22
#define SdDriver_ACMD_SEND_NUM_WR_BLOCKS_R SdDriver_R1
// SET_WR_BLK_ERASE_COUNT
#define SdDriver_ACMD_SET_WR_BLK_ERASE_COUNT 23
#define SdDriver_ACMD_SET_WR_BLK_ERASE_COUNT_R SdDriver_R1
// SEND_OP_COND
#define SdDriver_ACMD_SEND_OP_COND 41
#define SdDriver_ACMD_SEND_OP_COND_R SdDriver_R1
// SET_CLR_CARD_DETECT
#define SdDriver_ACMD_SET_CLR_CARD_DETECT 42
#define SdDriver_ACMD_SET_CLR_CARD_DETECT_R SdDriver_R1
// SEND_SCR
#define SdDriver_ACMD_SEND_SCR 51
#define SdDriver_ACMD_SEND_SCR_R SdDriver_R1

/**************************** STATE VARIABLES **********************/
extern uint16_t SdDriver_timeoutWrite;
extern uint16_t SdDriver_timeoutRead;
extern uint8_t SdDriver_busyFlag;
extern uint8_t SdDriver_commandResponse[5];

/**************************** FUNCTIONS ****************************/
void SdDriver_Delay(uint8_t number);
uint8_t SdDriver_SendCommand(uint8_t command, uint8_t responseType, uint32_t argument);
uint8_t SdDriver_Initialize(void);
uint8_t SdDriver_ReadSingleBlock(uint32_t startBlock, uint8_t *buffer);
uint8_t SdDriver_WriteSingleBlock(uint32_t startBlock, uint8_t *buffer);
uint8_t SdDriver_ReadMultipleBlock(uint32_t startBlock, uint32_t numberBlocks, uint8_t *buffer);
uint8_t SdDriver_WriteMultipleBlock(uint32_t startBlock, uint32_t numberBlocks, uint8_t *buffer);

#endif /* SDDRIVER_H_ */