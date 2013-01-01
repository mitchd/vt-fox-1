#include "spi_flash.h"

static uint8_t flash_addr[3];
static uint8_t flash_cmd;

static const SPIConfig spi1cfg = {
  NULL,         //Callback function
  GPIOA,        //NSS Port
  3,            //NSS Pad
  0
};

void configureSPIFlash(void){
  //Initialize the SPI port
  spiStart(&SPID1, &spi1cfg);

  //Setup the pads
  
  //Configure NSS
  palSetPad(GPIOA, 3);
  palSetPadMode( GPIOA, 3, PAL_MODE_OUTPUT_PUSHPULL |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure SCK
  palSetPadMode( GPIOA, 5, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST );

  //Configure MISO
  palSetPadMode( GPIOA, 6, PAL_MODE_ALTERNATE(5) );

  //Configure MOSI
  palSetPadMode( GPIOA, 7, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST );
}

static uint8_t checkBusy(void){
  uint8_t  flash_status = 0;
  //Send NSS Low
  spiSelect( &SPID1 );
  //Set the read status register command
  flash_cmd = FLASH_READ_STATUS;
  spiSend( &SPID1, 1, &flash_cmd );
  //Get the status
  spiReceive( &SPID1, 1, &flash_status );
  return (FLASH_STATUS_BUSY & flash_status); //Check for write complete
  spiUnselect( &SPID1 );
}

void flashWriteByte( uint32_t addr, uint8_t data ){
  //Ensure that we get a legitimate address
  addr &= FLASH_HIGH_ADDR-1;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0xFF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x00FF00)>>8;
  flash_addr[2] = (addr & 0x0000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
  //Wait until there is no write in progress
  while( checkBusy() );
  //Set the command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Prepare for data transfer
  spiSelect( &SPID1 );
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  //Transmit the byte write command
  flash_cmd = FLASH_BYTE_PROGRAM;
  spiSend( &SPID1, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID1, 3, flash_addr );
  //Transmit the byte
  spiSend( &SPID1, 1, &data );
  //Transmit the write disable command
  flash_cmd = FLASH_WRITE_DISABLE;
  spiSend( &SPID1, 1, &flash_cmd );
  //End the transfer
  spiUnselect( &SPID1 );
  //Release the bus
  spiReleaseBus( &SPID1 );

  return;
}

void flashWriteBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  const uint8_t zero_fill = 0x00;
  uint32_t tx_bytes = 2;
  //Ensure that we get a legitimate address
  addr &= FLASH_HIGH_ADDR-1;
  //Prevent overflow
  if( n > (FLASH_HIGH_ADDR - addr) )
    return;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0xFF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x00FF00)>>8;
  flash_addr[2] = (addr & 0x0000FF);     //LSB
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
  //Wait until there is no write in progress
  while( checkBusy() );
  //Software EndOfWrite Detection
  flash_cmd = FLASH_DBSY;
  //Prepare for data transfer
  spiSelect( &SPID1 );
  //Transmit software EOW Detection
  spiSend( &SPID1, 1, &flash_cmd );
  //Set the WEN command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  //AAI Command
  flash_cmd = FLASH_AAI_PROGRAM;
  //Transmit the AAI command
  spiSend( &SPID1, 1, &flash_cmd );
  //Transmit the destination address
  spiSend( &SPID1, 3, flash_addr );
  //Transmit the first two bytes
  spiSend( &SPID1, 2, &data );
  //Send NSS High
  spiUnselect( &SPID1 );
  //Loop through the rest of data
  while( tx_bytes < n ){
    if( !checkBusy() ){  //Check for write complete
      spiSelect( &SPID1 );
      //Set the AAI command
      flash_cmd = FLASH_AAI_PROGRAM;
      //Transmit the AAI command
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
    }
    //Send NSS High
    spiUnselect( &SPID1 );
  }
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
  //For this function, we'll use the high-speed read functionality
  uint8_t returnByte;
  uint8_t zero = 0x00;
  //Ensure that we get a legitimate address
  addr &= FLASH_HIGH_ADDR-1;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0xFF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x00FF00)>>8;
  flash_addr[2] = (addr & 0x0000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID1 );
  //Ensure there is no write in progress
  while( checkBusy() ); 
  //Set high speed read cmd
  flash_cmd = FLASH_HS_READ;
  //Send cmd
  spiSelect( &SPID1 );
  spiSend( &SPID1, 1, &flash_cmd );
  //Send address
  spiSend( &SPID1, 3, flash_addr );
  //Don't care byte
  spiSend( &SPID1, 1, &zero );
  //Read byte
  spiReceive( &SPID1, 1, &returnByte );
  //Release the device
  spiUnselect( &SPID1 );
  spiReleaseBus( &SPID1 );

  return returnByte;
}

void flashReadBytes( uint32_t addr, uint8_t* data, uint32_t n ){
  //For this function, we'll use the high-speed read functionality
  uint8_t zero = 0x00;
  //Ensure that we get a legitimate address
  addr &= FLASH_HIGH_ADDR-1;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0xFF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x00FF00)>>8;
  flash_addr[2] = (addr & 0x0000FF);     //LSB
  //Grab the SPI device
  spiAcquireBus( &SPID1 );
  //Ensure there is no write in progress
  while( checkBusy() ); 
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
