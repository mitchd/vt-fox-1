/*spi_flash.h
 *
 *
 * This file defines the interface for using ChibiOS/RT to communicate with
 * Microchip's SST SPI Serial Flash modules.  All functions are Thread Safe and
 * utilize ChibiOS/RT's built-in mutex capabilities to lock and unlock the SPI
 * device.  Mutexes *must* be enabled in the ChibiOS/RT configuration header.
 *
 * Currently this module is configured for the 8Mb module.
 *
 * uint32_t is used to store destination addresses, however the 8Mb module is
 * bit-addressable.  Only the lower 20 bits are used (three address bytes for
 * 24 total address bits).
 *
 * In the future 32Mb module, the address will require 22 bits (three address
 * bytes).
 *
 * The read operations use the "high speed read" mode so no clock division is
 * required on SCK.  Maximum data transfer rate in this mode is 66 MHz
 *
 * The sector and block erase functions take a complete address, and determine
 * the appropriate MSBs to command an erase (thus, if you specify an address
 * that is not a block boundary, the function will determine the block the
 * address is in).
 */



#ifndef __SPI_FLASH__
#include "ch.h"
#include "hal.h"
#define __SPI_FLASH__

//Opcodes for Microchip SST Flash
//Currently for their 8Mb module
#define FLASH_LOW_ADDR			0x000000  
#define FLASH_HIGH_ADDR			0x100000
#define FLASH_READ 			0x03  //Low speed read up to 33MHz
#define FLASH_HS_READ 			0x0B  //High speed read up to 66MHz
#define FLASH_4KB_ERASE			0x20  //Erase a 4k sector
#define FLASH_32KB_ERASE		0x52  //32k erase
#define FLASH_64KB_ERASE		0xD8  //64k erase
#define FLASH_CHIP_ERASE		0x60  //Erase whole chip
#define FLASH_BYTE_PROGRAM		0x02  //Program one byte
#define FLASH_AAI_PROGRAM		0xAD  //Auto-address-increment program
#define FLASH_READ_STATUS		0x05  //Read status register
#define FLASH_ENABLE_WRITE_STATUS	0x50  //Enable status reg write
#define FLASH_WRITE_STATUS		0x01  //Write to status reg
#define FLASH_WRITE_ENABLE		0x06  //Enable write
#define FLASH_WRITE_DISABLE		0x04  //Disable write (terminate AAI)
#define FLASH_READ_ID			0x90  //Read manufacturer ID
#define FLASH_READ_JDEC_ID		0x9F  //Read manufacturer ID
#define FLASH_EBSY			0x70  //Enable MISO write end detect
#define FLASH_DBSY			0x80  //Disable hardware write end 
                                              //  detect,enable software write 
                                              //  end detect

//Status Register Values
#define FLASH_STATUS_BUSY		0x01  //1 = write in progress
#define FLASH_STATUS_WEL		0x02  //1 = device write enabled
#define FLASH_STATUS_BP0		0x04  //------
#define FLASH_STATUS_BP1		0x08  //Indicate current level of
#define FLASH_STATUS_BP2		0x10  //block protection
#define FLASH_STATUS_BP3		0x20  //------
#define FLASH_STATUS_AAI		0x40  //1 = AAI mode 0 = Byte-program
#define FLASH_STATUS_BPL		0x80  //1 = BPx are read-only

//SPI Configuration Function
/*configureSPIFlash
 *
 * This function configures the SPI1 hardware module for communicating with the
 * Microchip SST Flash modules.  The following uC pins are used
 *   PA4  - NSS
 *   PA5 - SCK
 *   PA6 - MISO
 *   PA7 - MOSI
 *
 * One implementation note:
 * 
 * On poweron the SST flash's status register defaults to enabling all block
 * write protection.  This function *must* disable the write protect bits.
 * Additionally, the ~WP pin on the SST Flash *must* be pulled high.  If ~WP
 * is low, then the block write protect bits cannot be cleared.
 */
void 	configureSPIFlash(void);

//Single-Byte write
/*flashWriteByte
 *
 * Write the byte contained in data to the SST Flash to the address addr.
 *
 * The SST Flash uses only 20 address bits, addr is expected to have the right-
 * aligned address within it.  E.G. :
 *   addr & 0x000FFFFF == addr
 * should return true (otherwise you're exceeding the address space).
 *
 * The address pointed to at addr must be erased if it contains any data that is
 * not 0xFF.  This function does not automatically erase the byte.  Erase
 * functionality is provided by flashErase 
 */
void 	flashWriteByte( uint32_t addr, uint8_t data );

//Single-Byte Read
/*flashReadByte
 *
 * Read the byte contained in the SST Flash at address addr and return the value
 *
 * See the notes for flashWriteByte about proper address values.
 *
 * Right now this function uses the SST Flash's high speed read functionality
 * for up to 66MHz clock speeds.  This requires an extra byte of zeros
 * after the address is transmitted.  If the SPI bus runs slower than 33MHz,
 * this extra padding can be removed, and the read command switched to the
 * 'regular' speed reads.
 */
uint8_t flashReadByte( uint32_t addr );

//Multi-Byte write
/*flashWriteBytes
 *
 * Perform a multi-byte write to SST Flash beginning at addr.  data points to
 * a buffer of length >= n.  n bytes are written from data beginning at the
 * address that data points to.
 *
 * If addr+n is greater than the highest adressable byte, then this function
 * returns without writing anything.  This could probably be modified to a
 * couple behaviors: loop around to addr = 0x00 or return a boolean.
 *
 * This function makes use of the SST Flash AAI opcode, which allows for a
 * continuous stream of bytes (instead of sending a new address every time).
 */
void	flashWriteBytes( uint32_t addr, uint8_t* data, uint32_t n );

//Multi-Byte read
/*flashReadBytes
 *
 * Perform a multi-byte read from SST Flash beginning at addr.  data points to
 * a buffer of length >= n.  n bytes are read from the SST Flash and written to
 * data beginning at the address data points to.
 *
 * If addr+n is greater than the highest addressable byte, the SST Flash will
 * automatically loop around to 0x00.  This function does not check to determine
 * if this will happen.
 *
 * This function utilizes the SST Flash high speed read opcode, which requires
 * a zero-padding byte after the address for read speads up to 66MHz.  If the
 * SPI clock is <= 33MHz then this can be converted to the 'regular' read.
 */
void	flashReadBytes( uint32_t addr, uint8_t* data, uint32_t n );

//Flash Erase (specify which type of erase with the above macro defs)
/*flashErase
 *
 * Erase a sector, block, or entire chip as specified by erase_cmd for specified
 * address addr.
 *
 * The SST Flash (8Mb module) has several erase modes that are defined above
 * in define statements:
 *   #define FLASH_4KB_ERASE		0x20  //Erase a 4k sector
 *   #define FLASH_32KB_ERASE		0x52  //32k erase
 *   #define FLASH_64KB_ERASE		0xD8  //64k erase
 *   #define FLASH_CHIP_ERASE		0x60  //Erase whole chip
 *
 * The SST Flash chip automatically selects the most significant bits of the
 * specified sector/block to determine which bytes to clear.  Thus, addr need
 * not be specified at a sector/block boundary.
 *
 * Additional functionality is providied in this function with the 'wait'
 * argument.
 *
 * If wait is > 0 then flashErase will wait to return until the SST Flash status
 * register reports that the requested operation is complete.  If wait == 0 then
 * the function returns immediately after instructing the SST Flash to perform
 * the specified erase operation
 */
void	flashErase( uint32_t addr, uint8_t erase_cmd, uint8_t wait );

#endif
