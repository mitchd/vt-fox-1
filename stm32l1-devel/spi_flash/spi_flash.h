/*
This file is part of The Virginia Tech Amateur Radio Association's AMSAT FOX-1
experiment.

There are two main projects from which this code derives:

ChibiOS/RT 2.4.3
jpegant

ChibiOS/RT is unmodified, and retains its source structure in ./os/

jpegant is modified from its original version, and best efforts are made to
annotate the differences between the released jpegant and the contributions made
by VTARA.

All of the source code within this project is copyright (2012, 2013):

Joseph "Mitch" Davis, WQ3C, mitchd@vt.edu
Kevin Burns, KJ4SYL, kevinpb@vt.edu
Virginia Polytechnic Institute and State University

This entire project is licensed under the GNU Public License (GPL) Version 3:


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

//
//Line data for each "scan line" in the jpeg image
//
typedef struct{
  uint8_t  line_num;	// 0-79, horizontal line number of the image.  Each line
			//is 8 pixels tall
  uint32_t start_addr;  //Start address of the line data
  uint32_t end_addr;    //End address of the line data
  uint8_t  chksum;      //8-bit checksum of the line data
} line_data;

//
//system_data is written at the beginning of the MRAM memory space
//
typedef struct{
  line_data  line[60];
} system_data;

#define SYSTEM_DATA_ADDR		0x000100
#define IMAGE_DATA_START		0x000500

//Opcodes for Everspin MRAM MR25H10 
#define FLASH_LOW_ADDR			0x000000  
#define FLASH_HIGH_ADDR			0x100000
#define FLASH_READ 			0x03  //Read byte
#define FLASH_WRITE      		0x02  //Write byte
#define FLASH_READ_STATUS		0x05  //Read status register
#define FLASH_ENABLE_WRITE_STATUS	0x50  //Enable status reg write
#define FLASH_WRITE_STATUS		0x01  //Write to status reg
#define FLASH_SLEEP			0xB9  //Sleep mode
#define FLASH_WAKE			0xAB  //Wake flash
#define FLASH_WRITE_ENABLE		0x06  //Enable Write Latch
#define FLASH_WRITE_DISABLE		0x04  //Disable Write Latch
//Status Register Values
#define FLASH_STATUS_WEL		0x02  //1 = device write enabled
#define FLASH_STATUS_BP0		0x04  //Indicate current level of
#define FLASH_STATUS_BP1		0x08  //block protection
#define FLASH_STATUS_SRWD		0x80  //SRWD

//SPI Configuration Function
/*configureSPIFlash
 *
 * This function configures the SPI1 hardware module for communicating with the
 * Everspin MRAM Flash module.  The following uC pins are used
 *   PA4 - NSS
 *   PA5 - SCK
 *   PA6 - MISO
 *   PA7 - MOSI
 *
 * 
 */
void 	configureSPIFlash(void);

//Single-Byte write
/*flashWriteByte
 *
 * Write the byte contained in data to the MRAM Flash to the address addr.
 *
 * The MRAM Flash uses only 17 address bits, addr is expected to have the right-
 * aligned address within it.  E.G. :
 *   addr & 0x0001FFFFF == addr
 * should return true (otherwise you're exceeding the address space).
 *
 */
void 	flashWriteByte( uint32_t addr, uint8_t data );

//Single-Byte Read
/*flashReadByte
 *
 * Read the byte contained in the MRAM Flash at address addr and return the 
 * value
 *
 * See the notes for flashWriteByte about proper address values.
 *
 */
uint8_t flashReadByte( uint32_t addr );

//Multi-Byte write
/*flashWriteBytes
 *
 * Perform a multi-byte write to MRAM Flash beginning at addr.  data points to
 * a buffer of length >= n.  n bytes are written from data beginning at the
 * address that data points to.
 *
 * If addr+n is greater than the highest adressable byte, then this function
 * returns without writing anything.  This could probably be modified to a
 * couple behaviors: loop around to addr = 0x00 or return a boolean.
 *
 */
void	flashWriteBytes( uint32_t addr, uint8_t* data, uint32_t n );

//Multi-Byte read
/*flashReadBytes
 *
 * Perform a multi-byte read from MRAM Flash beginning at addr.  data points to
 * a buffer of length >= n.  n bytes are read from the SST Flash and written to
 * data beginning at the address data points to.
 *
 * If addr+n is greater than the highest addressable byte, the SST Flash will
 * automatically loop around to 0x00.  This function does not check to determine
 * if this will happen.
 *
 */
void	flashReadBytes( uint32_t addr, uint8_t* data, uint32_t n );


/* Read line of image data from SPI
 * - Determine addres of line_data struct in SPI memory
 * - Read contents of line_data struct into main memory
 * - Determine address/length of line data from line_data struct
 * - Copy from SPI to main memory at location data
 * - Validate checksum while doing so?
 */
uint16_t readLineFromSPI(int line, uint8_t *data);

#endif
