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
  4,           //NSS Pad
  0 		//Configuration Register stuff (set bits here to go slow)
};


static void flashSleep(void){
  uint8_t flash_cmd = FLASH_SLEEP;
  //NSS Low
  spiSelect( &SPID1 );
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  //Send NSS High (execute WEN)
  spiUnselect( &SPID1 );
}

static void flashWake(void){
  uint8_t flash_cmd = FLASH_WAKE;
  //NSS Low
  spiSelect( &SPID1 );
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  //Send NSS High (execute WEN)
  spiUnselect( &SPID1 );
}


void configureSPIFlash(void){
  //Initialize the SPI port
  spiStart(&SPID1, &spi1cfg);

  //Setup the pads
  
  //Configure NSS
  palSetPad(GPIOA, 4);
  palSetPadMode( GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure SCK
  palSetPadMode( GPIOA, 5, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure MISO
  palSetPadMode( GPIOA, 6, PAL_MODE_ALTERNATE(5) );

  //Configure MOSI
  palSetPadMode( GPIOA, 7, PAL_MODE_ALTERNATE(5) |
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
  flashSleep();
  spiReleaseBus( &SPID1 );
}


void flashWriteByte( uint32_t addr, uint8_t data ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00010000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
  flashWake();
  chThdSleepMicroseconds(400);
  //Wait until there is no write in progress
  //Set the command
  flash_cmd = FLASH_WRITE_ENABLE;
  //NSS Low
  spiSelect( &SPID1 );
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  //Send NSS High (execute WEN)
  spiUnselect( &SPID1 );
  flash_cmd = FLASH_WRITE;
  //Send NSS Low
  spiSelect( &SPID1 );
  //Transmit the byte write command
  spiSend( &SPID1, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID1, 3, &flash_addr );
  //Transmit the byte
  spiSend( &SPID1, 1, &data );
  //NSS High
  spiUnselect( &SPID1 );
  //Disable Write
  flash_cmd = FLASH_WRITE_DISABLE;
  //NSS Low
  spiSelect( &SPID1 );
  //Transmit the write disable command
  spiSend( &SPID1, 1, &flash_cmd );
  //Send NSS High (execute WRDI)
  spiUnselect( &SPID1 );
  //Release the bus
  flashSleep();
  spiReleaseBus( &SPID1 );

  return;
}

void flashWriteBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  //Prevent overflow
  if( n > (FLASH_HIGH_ADDR - addr) || n < 2)
    return;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00010000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
  flashWake();
  chThdSleepMicroseconds(400);
  //NSS Low
  spiSelect( &SPID1 );
  //Set the WEN command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  spiUnselect( &SPID1 );
  //Prepare to write data
  flash_cmd = FLASH_WRITE;
  spiSelect( &SPID1 );
  //Transmit the write command
  spiSend( &SPID1, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID1, 3, flash_addr );
  //Transmit the data
  spiSend( &SPID1, n, data );
  //Send NSS High
  spiUnselect( &SPID1 );
  //Send NSS Low
  spiSelect( &SPID1 );
  //Set write disable
  flash_cmd = FLASH_WRITE_DISABLE;
  spiSend( &SPID1, 1, &flash_cmd );
  //Send NSS high
  spiUnselect( &SPID1 );
  //Release the bus
  flashSleep();
  spiReleaseBus( &SPID1 );
  return;
}

uint8_t flashReadByte( uint32_t addr ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  uint8_t returnByte;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00010000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID1 );
  flashWake();
  chThdSleepMicroseconds(400);
  flash_cmd = FLASH_READ;
  //Send cmd
  spiSelect( &SPID1 );
  spiSend( &SPID1, 1, &flash_cmd );
  //Send address
  spiSend( &SPID1, 3, flash_addr );
  //Read byte
  spiReceive( &SPID1, 1, &returnByte );
  //Release the device
  spiUnselect( &SPID1 );
  flashSleep();
  spiReleaseBus( &SPID1 );

  return returnByte;
}

void flashReadBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  uint8_t flash_addr[3];
  uint8_t flash_cmd;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00010000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID1 );
  flashWake();
  chThdSleepMicroseconds(400);
  //Set high speed read cmd
  flash_cmd = FLASH_READ;
  //Send cmd
  spiSelect( &SPID1 );
  spiSend( &SPID1, 1, &flash_cmd );
  //Send address
  spiSend( &SPID1, 3, flash_addr );
  //Read bytes
  spiReceive( &SPID1, n, data );
  //Release the device
  spiUnselect( &SPID1 );
  flashSleep();
  spiReleaseBus( &SPID1 );

  return;
}

/* Read image line number _line_ from SPI flash,
 * write it to memory pointed to be _data_, allocated
 * by caller.  Returns length of copied line, or 0
 * in the case of an error.
 */

uint16_t readLineFromSPI(int line, uint8_t *data) {
  line_data *ld_spi = &((line_data*)SYSTEM_DATA_START)[line];
  line_data ld;
  uint32_t len;
  uint8_t acc;
  unsigned int i;

  /* Pull the line_data struct for this line from spi flash */
  flashReadBytes((uint32_t)ld_spi, (uint8_t*)&ld, sizeof(line_data));    
  /* Using info in line_data struct, read the actual line */
  len = ld.end_addr - ld.start_addr;
  flashReadBytes(ld.start_addr, data, len);
  /* Validate the checksum */
  
  for (i = 0, acc = 0; i < len; ++i) {
    acc += data[i];
  }

  if (acc != ld.chksum) {
    return 0;
  }

  return len;

}
      
