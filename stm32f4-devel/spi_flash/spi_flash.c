#include "spi_flash.h"

static uint8_t flash_addr[3];
static uint8_t flash_cmd;

void configureSPI_Flash(void){
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


void flashWriteByte( uint32_t addr, uint8_t data ){
  //Ensure that we get a legitimate address
  addr &= FLASH_HIGH_ADDR-1;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0xFF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x00FF00)>>8;
  flash_addr[2] = (addr & 0x0000FF);     //LSB
  //Set the command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
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
  uint8_t  flash_status = 0;
  //Ensure that we get a legitimate address
  addr &= FLASH_HIGH_ADDR-1;
  //Prevent overflow
  if( n > (FLASH_HIGH_ADDR - addr) )
    return;
  //Map addr into 8-bit words to send
  flash_addr[0] = (addr & 0xFF0000)>>16; //MSB
  flash_addr[1] = (addr & 0x00FF00)>>8;
  flash_addr[2] = (addr & 0x0000FF);     //LSB
  //Software EndOfWrite Detection
  flash_cmd = FLASH_DBSY;
  //Acquire the SPI device
  spiAcquireBus( &SPID1 );
  //Prepare for data transfer
  spiSelect( &SPID1 );
  //Transmit software EOW Detection
  spiSend( &SPID1, 1, &flash_cmd );
  //Set the WEN command
  flash_cmd = FLASH_WRITE_ENABLE;
  //Transmit the write enable command
  spiSend( &SPID1, 1, &flash_cmd );
  //AAI Command
  flash_cmd = FLASH_AAI_PROGRAM
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
    //Send NSS Low
    spiSelect( &SPID1 );
    //Set the read status register command
    flash_cmd = FLASH_READ_STATUS;
    spiSend( &SPID1, 1, &flash_cmd );
    //Get the status
    spiReceive( &SPID1, 1, &flash_status );
    if( ~(FLASH_STATUS_BUSF & flash_status) ){  //Check for write complete
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
