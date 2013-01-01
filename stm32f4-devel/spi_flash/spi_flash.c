#include "spi_flash.h"

static uint8_t flash_addr[3];
static uint8_t flash_cmd;

static const SPIConfig spi2cfg = {
  NULL,         //Callback function
  GPIOA,        //NSS Port
  3,            //NSS Pad
  0 		//Configuration Register stuff (set bits here to go slow)
};

static uint8_t checkBusy(void){
  uint8_t  flash_status = 0;
  //Send NSS Low
  spiSelect( &SPID2 );
  //Set the read status register command
  flash_cmd = FLASH_READ_STATUS;
  spiSend( &SPID2, 1, &flash_cmd );
  //Get the status
  spiReceive( &SPID2, 1, &flash_status );
  spiUnselect( &SPID2 );
  return (FLASH_STATUS_BUSY & flash_status); //Check for write complete
}

void configureSPIFlash(void){
  //Initialize the SPI port
  spiStart(&SPID2, &spi2cfg);

  //Setup the pads
  
  //Configure NSS
  palSetPad(GPIOA, 3);
  palSetPadMode( GPIOA, 3, PAL_MODE_OUTPUT_PUSHPULL |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure SCK
  palSetPadMode( GPIOB, 13, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure MISO
  palSetPadMode( GPIOB, 14, PAL_MODE_ALTERNATE(5) );

  //Configure MOSI
  palSetPadMode( GPIOB, 15, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST );

  //Disable write protection
  uint8_t spi_cmd = FLASH_ENABLE_WRITE_STATUS;
  spiAcquireBus( &SPID2 );
  spiSelect( &SPID2 );
  //Transmit WRSR
  spiSend( &SPID2, 1, &spi_cmd );
  spiUnselect( &SPID2 );
  //Transmit Destination Register
  spi_cmd = FLASH_WRITE_STATUS;
  spiSelect( &SPID2 );
  spiSend( &SPID2, 1, &spi_cmd );
  //Disable BP regs
  spi_cmd = 0x00;
  spiSend( &SPID2, 1, &spi_cmd );
  //Transmit the write disable command
  flash_cmd = FLASH_WRITE_DISABLE;
  spiSend( &SPID2, 1, &flash_cmd );
  spiUnselect( &SPID2 );
  spiReleaseBus( &SPID2 );
}


void flashWriteByte( uint32_t addr, uint8_t data ){
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00FF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID2 );
  //Wait until there is no write in progress
  while( checkBusy() );
  //Set the command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Prepare for data transfer
  spiSelect( &SPID2 );
  //Transmit the write enable command
  spiSend( &SPID2, 1, &flash_cmd );
  //Send NSS High (execute WEN)
  spiUnselect( &SPID2 );
  flash_cmd = FLASH_BYTE_PROGRAM;
  //Send NSS Low
  spiSelect( &SPID2 );
  //Transmit the byte write command
  spiSend( &SPID2, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID2, 3, flash_addr );
  //Transmit the byte
  spiSend( &SPID2, 1, &data );
  //Transmit the write disable command
  flash_cmd = FLASH_WRITE_DISABLE;
  spiSend( &SPID2, 1, &flash_cmd );
  //End the transfer
  spiUnselect( &SPID2 );
  //Release the bus
  spiReleaseBus( &SPID2 );

  return;
}

void flashWriteBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  const uint8_t zero_fill = 0x00;
  uint32_t tx_bytes = 2;
  //Prevent overflow
  if( n > (FLASH_HIGH_ADDR - addr) || n < 2)
    return;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00FF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID2 );
  //Wait until there is no write in progress
  while( checkBusy() );
  //NSS Low
  spiSelect( &SPID2 );
  //Set the WEN command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Transmit the write enable command
  spiSend( &SPID2, 1, &flash_cmd );
  spiUnselect( &SPID2 );
  //Software EndOfWrite Detection
  flash_cmd = FLASH_DBSY;
  //Prepare for data transfer
  spiSelect( &SPID2 );
  //Transmit software EOW Detection
  spiSend( &SPID2, 1, &flash_cmd );
  //AAI Command
  flash_cmd = FLASH_AAI_PROGRAM;
  //Transmit the AAI command
  spiSend( &SPID2, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID2, 3, flash_addr );
  //Transmit the first two bytes
  spiSend( &SPID2, 2, data );
  //Send NSS High
  spiUnselect( &SPID2 );
  //Loop through the rest of data
  while( tx_bytes < n ){
    if( !checkBusy() ){  //Check for write complete
      spiSelect( &SPID2 );
      //Set the AAI command
      flash_cmd = FLASH_AAI_PROGRAM;
      //Transmit the AAI command
      spiSend( &SPID2, 1, &flash_cmd );
      if( tx_bytes+1 == n ){ //Check to see if we need a fill
        //Transmit the last byte
        spiSend( &SPID2, 1, data+tx_bytes );
        //Transmit the fill
        spiSend( &SPID2, 1, &zero_fill );
        tx_bytes++;
      }else{
        spiSend( &SPID2, 2, data+tx_bytes );
        tx_bytes += 2;
      }
    }
    //Send NSS High
    spiUnselect( &SPID2 );
  }
  //Send NSS Low
  spiSelect( &SPID2 );
  //Set write disable
  flash_cmd = FLASH_WRITE_DISABLE;
  spiSend( &SPID2, 1, &flash_cmd );
  //Send NSS high
  spiUnselect( &SPID2 );
  //Release the bus
  spiReleaseBus( &SPID2 );
  return;
}

uint8_t flashReadByte( uint32_t addr ){
  //For this function, we'll use the high-speed read functionality
  uint8_t returnByte;
  uint8_t zero = 0x00;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00FF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID2 );
  //Ensure there is no write in progress
  while( checkBusy() ); 
  flash_cmd = FLASH_HS_READ;
  //Send cmd
  spiSelect( &SPID2 );
  spiSend( &SPID2, 1, &flash_cmd );
  //Send address
  spiSend( &SPID2, 3, flash_addr );
  //Send dummy
  spiSend( &SPID2, 1, &zero );
  //Read byte
  spiReceive( &SPID2, 1, &returnByte );
  //Release the device
  spiUnselect( &SPID2 );
  spiReleaseBus( &SPID2 );

  return returnByte;
}

void flashReadBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  //For this function, we'll use the high-speed read functionality
  const uint8_t zero = 0x00;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00FF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID2 );
  //Ensure there is no write in progress
  while( checkBusy() ); 
  //Set high speed read cmd
  flash_cmd = FLASH_HS_READ;
  //Send cmd
  spiSelect( &SPID2 );
  spiSend( &SPID2, 1, &flash_cmd );
  //Send address
  spiSend( &SPID2, 3, flash_addr );
  //Don't care byte
  spiSend( &SPID2, 1, &zero );
  //Read bytes
  spiReceive( &SPID2, n, data );
  //Release the device
  spiUnselect( &SPID2 );
  spiReleaseBus( &SPID2 );

  return;
}

void flashErase( uint32_t addr, uint8_t erase_cmd, uint8_t wait ){
  //The flash module (according to the datasheet), does its own AND operation
  //with the appropriate MSBs to determine the sector to erase, so we don't
  //have to
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0x00FF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x0000FF00)>>8;
  flash_addr[2] = (addr & 0x000000FF);     //LSB
  //Acquire the device
  spiAcquireBus( &SPID2 );
  //Ensure there is no write in progress
  while( checkBusy() );
  //Set the WREN command
  spiSelect( &SPID2 );
  flash_cmd = FLASH_WRITE_ENABLE;
  //Send WREN
  spiSend( &SPID2, 1, &flash_cmd );
  spiUnselect( &SPID2 );
  //NSS High, set erase command
  flash_cmd = erase_cmd;
  spiSelect( &SPID2 );
  //Send sector erase
  spiSend( &SPID2, 1, &flash_cmd );
  if( erase_cmd != FLASH_CHIP_ERASE ){
    //Send address
    spiSend( &SPID2, 3, flash_addr );
  }
  //NSS High to execute
  spiUnselect( &SPID2 );
  if( wait == TRUE )
    while( checkBusy() );
  //Release the device
  spiReleaseBus( &SPID2 );

  return;
}
