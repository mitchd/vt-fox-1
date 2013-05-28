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
#include "camera_iface.h"

//Timer Configurationfor bus communications
#define CLK_DELAY 640
#define FIFO_DELAY 5
#define PWR_DELAY 32000
static const GPTConfig gpt3cfg = {
  32000000,    /* 32MHz timer clock.*/
  NULL         /* Timer callback.*/
};

//Configuration addresses and bytes ensure CONFIG_PAIRS reflects the number
//of configuration pairs needed!
#define CONFIG_PAIRS 16
//
//30 FPS VGA RGB565 Mode
//
static const uint8_t cam_config[CONFIG_PAIRS][2] = {
  {CAM_CLKRC, 0x01},
  {CAM_COM3, 0x00}, //enable scaling
  {CAM_COM7, 0x04}, //RGB enabled
  {CAM_COM14, 0x00},
  {CAM_COM10, 0x02},  //VERY IMPORTANT VSYNC INVERT
  {CAM_RGB444, 0x00}, //Disable RGB444
  {CAM_COM15, 0xD0}, //RGB565 output
  {CAM_SCALING_DCWCTR, 0x11},
  {CAM_SCALING_PCK_DIV, 0xF0},
  {CAM_SCALING_PCK_DELAY, 0x00 },
  {CAM_SCALING_XSC, 0x3A },
  {CAM_SCALING_YSC, 0x4A },
  {CAM_MVFP, 0x31}, //Flip horizontally and vertically
  {CAM_HREF, 0x04 | 0x04<<3 | 0x80}, //Setup the Horizontal window
  {CAM_HSTART, 0x0E},  //Columns 116 through 756
  {CAM_HSTOP, 0x3E}
};


//Setup the SCCB controller
void setupSCCB(void){
  //SCL
  palSetPadMode(CAM_CTL_PORT, CAM_SCL, PAL_MODE_OUTPUT_PUSHPULL |
                                       PAL_STM32_OSPEED_LOWEST );
  //SDA
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_OUTPUT_PUSHPULL |
                                       PAL_STM32_OSPEED_LOWEST );
}

//Use this to turn on the timer
static void prepareTimer(void){
  gptStart(&GPTD2, &gpt3cfg);
  return;
}

//Don't forget to turn it off when we're done transferring data
static void deactivateTimer(void){
  gptStop(&GPTD2);
  return;
}

//Initiate SCCB start condition -- requires the timer
static void startCondition(void){
  chSysLockFromIsr();

  //Pull SDA SCL High
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  //Wait
  gptPolledDelay(&GPTD2,CLK_DELAY);
  //Data Low
  palWritePad(CAM_CTL_PORT, CAM_SDA, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  //Clock Low
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);

  chSysUnlockFromIsr();
  return;
}

//Release the SCCB -- requires the timer
static void stopCondition(void){
  chSysLockFromIsr();
  //Pull SDA Low
  palWritePad(CAM_CTL_PORT, CAM_SDA, 0);
  //Set SCL Low
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);

  gptPolledDelay(&GPTD2,CLK_DELAY);
  //SCL High
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  //SDA High
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);

  chSysUnlockFromIsr();
  return;
}

//Idle the SCCB
static void idleState(void){
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);
  return;
}
 
//Write a byte, MSB first, with don't-care at the end -- requires the timer
static void writeByte(uint8_t byte){

  chSysLockFromIsr();
  //MSB Bit 7
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x80)>>7);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 6
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x40)>>6);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 5
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x20)>>5);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 4
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x10)>>4);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  
  //Bit 3
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x08)>>3);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
 
  //Bit 2
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x04)>>2);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 1
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x02)>>1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 0
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x01) );
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  

  //9th Don't Care bit
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  //Deassert don't care
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_INPUT );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  
  chSysUnlockFromIsr();
  //Re-assert the SDA
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_OUTPUT_PUSHPULL |
                                       PAL_STM32_OSPEED_LOWEST );
  //Done with byte
  return;
}

static uint8_t readByte(void){ 
  chSysLockFromIsr();
  
  //Configure input pad 
  uint8_t byte = 0;
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_INPUT );
  
  gptPolledDelay(&GPTD2,CLK_DELAY);

  //MSB Bit 7
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<7;

  //Bit 6
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<6;

  //Bit 5
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<5;

  //Bit 4
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<4;

  //Bit 3
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<3;

  //Bit 2
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<2;

  //Bit 1
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<1;

  //Bit 0
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) );

  //9th Don't Care Byte
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  //Dive to 1 per datasheet
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_OUTPUT_PUSHPULL |
                                       PAL_STM32_OSPEED_LOWEST );
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);  
  gptPolledDelay(&GPTD2,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD2,CLK_DELAY);
  chSysUnlockFromIsr();
  //Re-assert the SDA
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_OUTPUT_OPENDRAIN |
                                       PAL_STM32_OSPEED_LOWEST );
  //Done with byte
  return byte;
}
//Read reg => retval using two-phase write and read cycle -- requires the timer
static uint8_t cameraReadCycle( uint8_t reg ){
  
  uint8_t byte;

  //Two phase write
  startCondition();
  writeByte(CAM_ADDR_W);
  writeByte(reg);
  stopCondition();

  idleState();

  //Two phase read
  startCondition();
  writeByte(CAM_ADDR_R);
  byte = readByte();
  stopCondition();

  return byte;
}

//Write reg <= value using the three-phase transmission cycle in the SCCB
//datasheet
static void cameraWriteCycle( uint8_t reg, uint8_t value ){
  
  //Three phase write
  startCondition();
  writeByte(CAM_ADDR_W);
  writeByte(reg);
  writeByte(value);
  stopCondition();
  return;
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
  palClearPad(FIFO_CTL_PORT, FIFO_WEN);
  palSetPadMode(FIFO_CTL_PORT, FIFO_RRST, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(FIFO_CTL_PORT, FIFO_RRST);
  palSetPadMode(FIFO_CTL_PORT, FIFO_OE, PAL_MODE_OUTPUT_PUSHPULL);
  palClearPad(FIFO_CTL_PORT, FIFO_OE);
  palSetPadMode(FIFO_CTL_PORT, FIFO_RCLK, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(FIFO_CTL_PORT, FIFO_RCLK);
  //powerdownCam();
  return;
}

//Check camera sanity
msg_t checkCameraSanity(void){
  uint8_t rxbyte;
  uint8_t tmp;
  //Verify Programming
  for( tmp = 0; tmp < CONFIG_PAIRS; tmp++ ){
    rxbyte = cameraReadCycle( cam_config[tmp][0] );
    //chprintf((BaseChannel *)&SD1, "ADDR: %x VAL: %x\r\n", cam_config[tmp][0], rxbyte );
    if( rxbyte != cam_config[tmp][1] ){
      chprintf(IHU_UART, "ADDR: %x ASSIGNED: %x ACTUAL: %x\r\n",cam_config[tmp][0],cam_config[tmp][1],rxbyte);
      return -1;
    }
  }

  return RDY_OK;
}


//Configure the camera
msg_t configureCam(void){
  uint8_t tmp;
  idleState();
  //Poweron the camera
  wakeupCam();
  gptPolledDelay(&GPTD2,PWR_DELAY);
  //Set Default Values
  cameraWriteCycle( CAM_COM7, 0x80 );
  cameraWriteCycle( CAM_CLKRC, 0x80 );
  //Configure the camera
  for( tmp = 0; tmp < CONFIG_PAIRS; tmp++ )
    cameraWriteCycle( cam_config[tmp][0], cam_config[tmp][1] );
  tmp = checkCameraSanity();
  //powerdownCam();
  return (msg_t)tmp;
}

void wakeupCam(){
  palClearPad(CAM_PORT, CAM_PWDN);
}

void powerdownCam(){
  palSetPad(CAM_PORT, CAM_PWDN);
}

msg_t setupSegment( uint8_t segment )
{ 
 
  uint8_t vstrt, vstop, vref;
  if( segment == 0 ){ //"Upper" Half of image
    vstrt = 0x02;  //Row 10 to 250
    vstop = 0x3E;
    vref = 0x00 | 0x03 | 0x03<<2;
  }else{ //"Lower" Half of image
    vstrt = 0x3F;  //Row 252 to 490
    vstop = 0x7A;
    vref = 0x00 | 0x00 | 0x03<<2;
  }

  //Write our window configuration to the control registers
  cameraWriteCycle( CAM_VREF, vref );
  cameraWriteCycle( CAM_VSTART, vstrt );
  cameraWriteCycle( CAM_VSTOP, vstop );

  //Verify write operations were successful
  uint8_t tmp;
  tmp = cameraReadCycle( CAM_VREF );
  chprintf(IHU_UART,"%x\t%x\r\n",vref,tmp);
  if( tmp != vref )
    return -1;

  tmp = cameraReadCycle( CAM_VSTART );
  if( tmp != vstrt )
    return -1;

  tmp = cameraReadCycle( CAM_VSTOP );
  if( tmp != vstop )
    return -1;
  
  return RDY_OK;
}
 

void fifoGrabBytes( uint8_t *buf, uint32_t n ){
  uint8_t dataByte = 0;
  uint32_t i = 0;
  for( i=0; i<n; i++ ){
    palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay( &GPTD2, FIFO_DELAY );
    palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay( &GPTD2, FIFO_DELAY );
    buf[i] = (palReadPort(FIFO_DATA_PORT) & 0xFF);
  }
}

static void resetReadPointer(void){
  // ~RRST
  palClearPad( FIFO_CTL_PORT, FIFO_RRST );
  gptPolledDelay( &GPTD2, FIFO_DELAY );
  palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD2, FIFO_DELAY );
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD2, FIFO_DELAY );
  // RRST High
  palSetPad( FIFO_CTL_PORT, FIFO_RRST );
  gptPolledDelay( &GPTD2, FIFO_DELAY );
  palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD2, FIFO_DELAY );
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD2, FIFO_DELAY );
}


msg_t cameraControlThread(void* arg){
  const uint16_t IMG_WIDTH = 640;
  const uint16_t IMG_HEIGHT = 480;
  const uint16_t NUM_READS = 30;
  chprintf(IHU_UART,"Setup Timer\r\n");
  prepareTimer();
  chprintf(IHU_UART,"Setup Cam Port\r\n");
  setupCamPort();

  uint8_t pixelData[IMG_WIDTH*2*8];
  chprintf(IHU_UART,"Setup SCCB Interface\r\n");
  setupSCCB();

  chprintf(IHU_UART,"Configure Camera\r\n");
  msg_t success = configureCam();
  //Wakeup Camera
  wakeupCam();
  //Wait 1ms per datasheet
  gptPolledDelay( &GPTD2, PWR_DELAY );
  if( success == RDY_OK ){  
    //Ensure WEN is disabled
    palClearPad( FIFO_CTL_PORT, FIFO_WEN );
    //jpeg_init();
    if( setupSegment( 1 ) != RDY_OK ){
      //segmentNumber--;
      chprintf(IHU_UART,"SETUP FAILED\r\n");
    }else{
      //Reset the read pointer
      resetReadPointer();
      //VSYNC HIGH = Frame Transmitting (inverted vsync)
      while( palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      //Turn on WEN to capture the next frame
      palSetPad( FIFO_CTL_PORT, FIFO_WEN );
      //VSYNC Low = Beginning of Frame - WRST is pulled low to reset write reg
      while( !palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      //Wait until VSYNC goes low again
      while( palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      //Disable WEN
      palClearPad( FIFO_CTL_PORT, FIFO_WEN );
      //Poweroff cam
      //powerdownCam();
      uint16_t bulk_reads;
      for( bulk_reads=0; bulk_reads < NUM_READS; bulk_reads++ ){
        fifoGrabBytes( pixelData, IMG_WIDTH*2*8 );
        //convert_rows( pixelData );
        sdWrite( IHU_UART_DEV, &pixelData[0], IMG_WIDTH*2*8 );
      }
    }
    //Wakeup Cam
    wakeupCam();
    //Wait 1sm per datasheet
    gptPolledDelay( &GPTD2, PWR_DELAY );
    if( setupSegment( 2 ) != RDY_OK ){
      //segmentNumber--;
      chprintf(IHU_UART,"SETUP FAILED\r\n");
    }else{
      //Reset the read pointer
      resetReadPointer();
      //VSYNC HIGH = Frame Transmitting (inverted vsync)
      while( palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      //Turn on WEN to capture the next frame
      palSetPad( FIFO_CTL_PORT, FIFO_WEN );
      //VSYNC Low = Beginning of Frame - WRST is pulled low to reset write reg
      while( !palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      //Wait until VSYNC goes low again
      while( palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      //Disable WEN
      palClearPad( FIFO_CTL_PORT, FIFO_WEN );
      //Poweroff Cam
      //powerdownCam();
      uint16_t bulk_reads;
      for( bulk_reads=0; bulk_reads < NUM_READS; bulk_reads++ ){
        fifoGrabBytes( pixelData, IMG_WIDTH*2*8 );
        //convert_rows( pixelData );
        sdWrite( IHU_UART_DEV, &pixelData[0], IMG_WIDTH*2*8 );
      }
    }
    //jpeg_close();
  }else{
    chprintf(IHU_UART,"SETUP FAILED\r\n");
  }
  while(TRUE); 
}
