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

#include "spi_flash.h"


static const SPIConfig spi1cfg = {
  NULL,         //Callback function
  GPIOA,        //NSS Port
  15,            //NSS Pad
  0 		//Configuration Register stuff (set bits here to go slow)
};

static void  checkBusy(void){
  uint8_t flash_cmd;
  uint8_t  flash_status = 0;
  //Send NSS Low
  spiSelect( &SPID1 );
  //Set the read status register command
  flash_cmd = FLASH_READ_STATUS;
  spiSend( &SPID1, 1, &flash_cmd );
  do{
    //Get the status
    spiReceive( &SPID1, 1, &flash_status );
  }while(FLASH_STATUS_BUSY & flash_status );
  spiUnselect( &SPID1 );
  return; //Check for write complete
}

void configureSPIFlash(void){
  //Initialize the SPI port
  spiStart(&SPID1, &spi1cfg);

  //Setup the pads
  
  //Configure NSS
  palSetPad(GPIOA, 15);
  palSetPadMode( GPIOA, 15, PAL_MODE_OUTPUT_PUSHPULL |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure SCK
  palSetPadMode( GPIOB, 3, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure MISO
  palSetPadMode( GPIOB, 4, PAL_MODE_ALTERNATE(5) );

  //Configure MOSI
  palSetPadMode( GPIOB, 5, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST );

  //Disable write protection
  uint8_t spi_cmd = FLASH_ENABLE_WRITE_STATUS;
  spiAcquireBus( &SPID1 );
  spiSelect( &SPID1 );
  //Transmit WRSR
  spiSend( &SPID1, 1, &spi_cmd );
  spiUnselect( &SPID1 );
  //Transmit Destination Register
  spi_cmd = FLASH_WRITE_STATUS;
  spiSelect( &SPID1 );
  spiSend( &SPID1, 1, &spi_cmd );
  //Disable BP regs
  spi_cmd = 0x00;
  spiSend( &SPID1, 1, &spi_cmd );
  //Transmit the write disable command
  spi_cmd = FLASH_WRITE_DISABLE;
  spiSend( &SPID1, 1, &spi_cmd );
  spiUnselect( &SPID1 );
  //Wait until the device is ready before releasing to the program
  checkBusy();
  spiReleaseBus( &SPID1 );
}


void flashWriteByte( uint32_t addr, uint8_t data ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x000F0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
  //Wait until there is no write in progress
  checkBusy();
  //Set the command
  flash_cmd = FLASH_WRITE_ENABLE;
  //NSS Low
  spiSelect( &SPID1 );
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  //Send NSS High (execute WEN)
  spiUnselect( &SPID1 );
  flash_cmd = FLASH_BYTE_PROGRAM;
  //Send NSS Low
  spiSelect( &SPID1 );
  //Transmit the byte write command
  spiSend( &SPID1, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID1, 3, flash_addr );
  //Transmit the byte
  spiSend( &SPID1, 1, &data );
  //NSS High
  spiUnselect( &SPID1 );
  //Release the bus
  spiReleaseBus( &SPID1 );

  return;
}

void flashWriteBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  const uint8_t zero_fill = 0x00;
  uint32_t tx_bytes = 2;
  //Prevent overflow
  if( n > (FLASH_HIGH_ADDR - addr) || n < 2)
    return;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x000F0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
  //Wait until there is no write in progress
  checkBusy();
  //NSS Low
  spiSelect( &SPID1 );
  //Set the WEN command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  spiUnselect( &SPID1 );
  //AAI Command
  flash_cmd = FLASH_AAI_PROGRAM;
  spiSelect( &SPID1 );
  //Transmit the AAI command
  spiSend( &SPID1, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID1, 3, flash_addr );
  //Transmit the first two bytes
  spiSend( &SPID1, 2, data );
  //Send NSS High
  spiUnselect( &SPID1 );
  //Loop through the rest of data
  while( tx_bytes < n ){
    checkBusy();
    //Transmit the AAI command
    flash_cmd = FLASH_AAI_PROGRAM;
    spiSelect( &SPID1 );
    spiSend( &SPID1, 1, &flash_cmd );
    if( tx_bytes+1 == n ){ //Check to see if we need a fill
      //Transmit the last byte
      spiSend( &SPID1, 1, data+tx_bytes );
      //Transmit the fill
      spiSend( &SPID1, 1, &zero_fill );
      tx_bytes++;
    }else{
      spiSend( &SPID1, 2, data+tx_bytes );
      tx_bytes += 2;
    }
    //Send NSS High
    spiUnselect( &SPID1 );
  }
  checkBusy();
  //Send NSS Low
  spiSelect( &SPID1 );
  //Set write disable
  flash_cmd = FLASH_WRITE_DISABLE;
  spiSend( &SPID1, 1, &flash_cmd );
  //Send NSS high
  spiUnselect( &SPID1 );
  //Release the bus
  spiReleaseBus( &SPID1 );
  return;
}

uint8_t flashReadByte( uint32_t addr ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  //For this function, we'll use the high-speed read functionality
  uint8_t returnByte;
  uint8_t zero = 0x00;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x000F0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID1 );
  //Ensure there is no write in progress
  checkBusy(); 
  flash_cmd = FLASH_HS_READ;
  //Send cmd
  spiSelect( &SPID1 );
  spiSend( &SPID1, 1, &flash_cmd );
  //Send address
  spiSend( &SPID1, 3, flash_addr );
  //Send dummy
  spiSend( &SPID1, 1, &zero );
  //Read byte
  spiReceive( &SPID1, 1, &returnByte );
  //Release the device
  spiUnselect( &SPID1 );
  spiReleaseBus( &SPID1 );

  return returnByte;
}

void flashReadBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  //For this function, we'll use the high-speed read functionality
  const uint8_t zero = 0x00;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x000F0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID1 );
  //Ensure there is no write in progress
  checkBusy(); 
  //Set high speed read cmd
  flash_cmd = FLASH_HS_READ;
  //Send cmd
  spiSelect( &SPID1 );
  spiSend( &SPID1, 1, &flash_cmd );
  //Send address
  spiSend( &SPID1, 3, flash_addr );
  //Don't care byte
  spiSend( &SPID1, 1, &zero );
  //Read bytes
  spiReceive( &SPID1, n, data );
  //Release the device
  spiUnselect( &SPID1 );
  spiReleaseBus( &SPID1 );

  return;
}

void flashErase( uint32_t addr, uint8_t erase_cmd, uint8_t wait ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  //The flash module (according to the datasheet), does its own AND operation
  //with the appropriate MSBs to determine the sector to erase, so we don't
  //have to
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x000F0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the device
  spiAcquireBus( &SPID1 );
  //Ensure there is no write in progress
  checkBusy();
  //Set the WREN command
  spiSelect( &SPID1 );
  flash_cmd = FLASH_WRITE_ENABLE;
  //Send WREN
  spiSend( &SPID1, 1, &flash_cmd );
  spiUnselect( &SPID1 );
  //NSS High, set erase command
  flash_cmd = erase_cmd;
  spiSelect( &SPID1 );
  //Send sector erase
  spiSend( &SPID1, 1, &flash_cmd );
  if( erase_cmd != FLASH_CHIP_ERASE ){
    //Send address
    spiSend( &SPID1, 3, flash_addr );
  }
  //NSS High to execute
  spiUnselect( &SPID1 );
  if( wait == TRUE )
    checkBusy();
  //Release the device
  spiReleaseBus( &SPID1 );

  return;
}
