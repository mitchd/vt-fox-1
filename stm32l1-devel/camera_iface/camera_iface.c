#include "camera_iface.h"

uint8_t ticks = 0;

/*
 * * GPT3 callback.
 * */
static void gpt3cb(GPTDriver *gptp)
{
  (void)gptp;
  ticks += 1;
}

static const GPTConfig gpt3cfg = {
  200000,    /* 100KHz timer clock.*/
  gpt3cb    /* Timer callback.*/
};

//Setup the SCCB controller
void setupSCCB(void){
  //SCL
  palSetPadMode(CAM_CTL_PORT, CAM_SCL, PAL_MODE_OUTPUT_OPENDRAIN |
                                       PAL_STM32_OSPEED_HIGHEST );
  //SDA
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_OUTPUT_OPENDRAIN |
                                       PAL_STM32_OSPEED_HIGHEST );
}

//Use this to turn on the timer
static inline prepareTimer(void){
  gptStart(&GPT3, &gpt3cfg);
}

//Don't forget to turn it off when we're done transferring data
static inline deactivateTimer(void){
  gptStop(&GPT3);
}

//Initiate SCCB start condition -- requires the timer
static inline startCondition(void){
  ticks = 0;
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);
  while( ticks < 1 );
  palWritePad(CAM_CTL_PORT, CAM_SDA, 0);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
}

//Release the SCCB -- requires the timer
static inline stopCondition(void){
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  ticks = 0;
  while( ticks < 1);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);
  while( ticks < 2);
}

//Idle the SCCB
static inline idleState(void){
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);
}
 
//Write a byte, MSB first, with don't-care at the end -- requires the timer
static inline writeByte(uint8_t byte){
  ticks = 0;
  idleState();
  while( ticks < 1 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x80)>>7);
  while( ticks < 2 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 3 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x40)>>6);
  while( ticks < 4 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 5 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x20)>>5);
  while( ticks < 6 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 7 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x10)>>4);
  while( ticks < 8 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 9 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x08)>>3);
  while( ticks < 10 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 11 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x04)>>2);
  while( ticks < 12 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 13 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x02)>>1);
  while( ticks < 14 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 15 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x01) );
  while( ticks < 16 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 17 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 18 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  while( ticks < 19 );
}

//Write reg <= value using the three-phase transmission cycle in the SCCB
//datasheet
static void cameraWriteCycle( uint8_t reg, uint8_t value ){
  startCondition();
  writeByte(CAM_ADDR_W);
  writeByte(reg);
  writeByte(value);
  stopCondition();
  return;
}

static uint8_t readByte(){ 
  uint8_t byte = 0;
  ticks = 0;
  idleState();
  while( ticks < 1 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 2 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<7;
  while( ticks < 3 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 4 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<6;
  while( ticks < 5 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 6 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<5;
  while( ticks < 7 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 8 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<4;
  while( ticks < 9 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 10 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<3;
  while( ticks < 11 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 12 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<2;
  while( ticks < 13 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 14 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<1;
  while( ticks < 15 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 16 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) );
  while( ticks < 17 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  while( ticks < 18 );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
}
//Read reg => retval using two-phase write and read cycle -- requires the timer
static uint8_t cameraReadCycle( uint8_t reg ){
  uint8_t byte;
  startCondition();
  writeByte(CAM_ADDR_W);
  writeByte(reg);
  stopCondition();
  ticks = 0;

  idleState();
  while( ticks < 4 );

  startCondition();
  writeByte(CAM_ADDR_R);
  byte = readByte();
  stopCondition();
  return byte;
}
//Configure the camera pads
void setupCamPort(void){
  palSetPadMode(CAM_PORT, CAM_XCLK, PAL_MODE_ALTERNATE(0) | 
                                    PAL_STM32_OSPEED_HIGHEST);
  palSetPadMode(CAM_PORT, CAM_RESET, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(CAM_PORT, CAM_PWDN, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(CAM_PORT2, CAM_VSYNC_OUT, PAL_MODE_INPUT);
  palSetPadMode(CAM_PORT2, CAM_HREF_OUT, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D0, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D1, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D2, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D3, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D4, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D5, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D6, PAL_MODE_INPUT);
  palSetPadMode(FIFO_DATA_PORT, FIFO_D7, PAL_MODE_INPUT);
  palSetPadMode(FIFO_CTL_PORT, FIFO_WEN, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(FIFO_CTL_PORT, FIFO_RRST, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(FIFO_CTL_PORT, FIFO_OE, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(FIFO_CTL_PORT, FIFO_RCLK, PAL_MODE_OUTPUT_PUSHPULL);
}

//Configure the camera
msg_t configureCam(void){
  uint8_t tx_buf[32];
  uint8_t rx_buf[4];
  uint8_t address;
  msg_t msg_value;
  //30 fps VGA YUV Mode
  tx_buf[0] = CAM_CLKRC;
  tx_buf[1] = 0x01;
  tx_buf[2] = CAM_COM7;
  tx_buf[3] = 0x00;
  tx_buf[4] = CAM_COM3;
  tx_buf[5] = 0x00;
  tx_buf[6] = CAM_COM14;
  tx_buf[7] = 0x00;
  tx_buf[8] = CAM_SCALING_XSC;
  tx_buf[9] = 0x3A;
  tx_buf[10] = CAM_SCALING_YSC;
  tx_buf[11] = 0x35;
  tx_buf[12] = CAM_SCALING_DCWCTR;
  tx_buf[13] = 0x11;
  tx_buf[14] = CAM_SCALING_PCK_DIV;
  tx_buf[15] = 0xF0;
  tx_buf[16] = CAM_SCALING_PCK_DELAY;
  tx_buf[17] = 0xA2;

  //Poweron the camera
  wakeupCam();

  //Grab the bus
  i2cAcquireBus(&I2CD1);
  address = CAM_I2C_ADDR;
  msg_value = i2cMasterTransmit( &I2CD1, address,
                                 tx_buf, 2,
                                 rx_buf, 0 );
  msg_value = i2cMasterTransmit( &I2CD1, address,
                                 tx_buf, 1,
                                 rx_buf, 0 );
  msg_value = i2cMasterReceive( &I2CD1, address,
                                rx_buf, 1 );
  chprintf((BaseChannel *)&SD1, "RX Byte %d\r\n", rx_buf[0] );
  i2cReleaseBus(&I2CD1);

  //Poweroff camera
  powerdownCam();
  if( rx_buf[0] == tx_buf[1] ){
    return RDY_OK;
  }
  return msg_value;
 

}

void wakeupCam(){
  //palClearPad(CAM_PORT, CAM_PWDN);
}

void powerdownCam(){
  //palSetPad(CAM_PORT, CAM_PWDN);
}

msg_t cameraControlThread(void* arg){
  chprintf((BaseChannel *)&SD1, "Beginning camera control thread\r\n");
  setupI2C();
  setupCamPort();
  while(TRUE){
    //chprintf((BaseChannel *)&SD1, "Inside camera thread\r\n");
    chThdSleepMilliseconds(2000);
    msg_t success = configureCam();
    chprintf((BaseChannel *)&SD1, "Camera config status: %d\r\n", success);
    if( success == RDY_OK ){
       chprintf((BaseChannel *)&SD1, "Config TX OK\r\n");
    }else if( success == RDY_RESET ){
      chprintf((BaseChannel *)&SD1, "I2C Errors Occured\r\n");
      i2cflags_t errors = i2cGetErrors(&I2CD1);
      if( errors & 0x00 )
        chprintf((BaseChannel *)&SD1, "No Errors\r\n");
      if( errors & 0x01 )
        chprintf((BaseChannel *)&SD1, "Bus Error\r\n");
      if( errors & 0x02 )
        chprintf((BaseChannel *)&SD1, "Arbitration Lost\r\n");
      if( errors & 0x04 )
        chprintf((BaseChannel *)&SD1, "ACK Failure\r\n");
      if( errors & 0x08 )
        chprintf((BaseChannel *)&SD1, "Overrun\r\n");
      if( errors & 0x10 )
        chprintf((BaseChannel *)&SD1, "PEC Error\r\n");
      if( errors & 0x20 )
        chprintf((BaseChannel *)&SD1, "Timeout\r\n");
      if( errors & 0x40 )
        chprintf((BaseChannel *)&SD1, "SMBus Alert\r\n");
    }else if( success == RDY_TIMEOUT ){
      chprintf((BaseChannel *)&SD1, "I2C Timeout Encountered\r\n");
    }else{
      chprintf((BaseChannel *)&SD1, "Unknown state returned\r\n");
    }
  }
}
