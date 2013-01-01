#ifndef __SPI_FLASH__
#include "ch.h"
#include "hal.h"
#include "stm32f4xx.h"
#include "chprintf.h"
#define __SPI_FLASH__

//Opcodes for Microchip SST Flash
//Currently for their 8Mb module
#define FLASH_LOW_ADDR			0x000000
#define FLASH_HIGH_ADDR			0x800000
#define FLASH_READ 			0x03
#define FLASH_HS_READ 			0x0B
#define FLASH_4KB_ERASE			0x20
#define FLASH_32KB_ERASE		0x52
#define FLASH_64KB_ERASE		0xD8
#define FLASH_CHIP_ERASE		0x60
#define FLASH_BYTE_PROGRAM		0x02
#define FLASH_AAI_PROGRAM		0xAD
#define FLASH_READ_STATUS		0x05
#define FLASH_ENABLE_WRITE_STATUS	0x50
#define FLASH_WRITE_STATUS		0x01
#define FLASH_WRITE_ENABLE		0x06
#define FLASH_WRITE_DISABLE		0x04
#define FLASH_READ_ID			0x90
#define FLASH_READ_JDEC_ID		0x9F
#define FLASH_EBSY			0x70
#define FLASH_DBSY			0x80


//Status Register Values
#define FLASH_STATUS_BUSY		0x01  //1 = write in progress
#define FLASH_STATUS_WEL		0x02  //1 = device write enabled
#define FLASH_STATUS_BP0		0x04  //------
#define FLASH_STATUS_BP1		0x08  //Indicate current level of
#define FLASH_STATUS_BP2		0x10  //block protection
#define FLASH_STATUS_BP3		0x20  //------
#define FLASH_STATUS_AAI		0x40  //1 = AAI mode 0 = Byte-program
#define FLASH_STATUS_BPL		0x80  //1 = BPx are read-only

void 	configureSPIFlash(void);
void 	flashWriteByte( uint32_t addr, uint8_t data );
uint8_t flashReadByte( uint32_t addr );
void	flashWriteBytes( uint32_t addr, uint8_t* data, uint32_t n );
void	flashReadBytes( uint32_t addr, uint8_t* data, uint32_t n );
#endif
