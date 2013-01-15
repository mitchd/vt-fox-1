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
static const GPTConfig gpt3cfg = {
  32000000,    /* 32MHz timer clock.*/
  NULL         /* Timer callback.*/
};

//Configuration addresses and bytes ensure CONFIG_PAIRS reflects the number
//of configuration pairs needed!
#define CONFIG_PAIRS 9
//30FPS YUV:  Y U Y V Y U Y V....
static const uint8_t cam_config[CONFIG_PAIRS][2] = {
  {CAM_CLKRC, 0x01},
  {CAM_COM7, 0x00},
  {CAM_COM3, 0x00},
  {CAM_COM14, 0x00},
  {CAM_SCALING_XSC, 0x3A},
  {CAM_SCALING_YSC, 0x35},
  {CAM_SCALING_DCWCTR, 0x11},
  {CAM_SCALING_PCK_DIV, 0xF0},
  {CAM_SCALING_PCK_DELAY, 0x02}
};

uint8_t pixelData[640*2];

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
  gptStart(&GPTD3, &gpt3cfg);
  return;
}

//Don't forget to turn it off when we're done transferring data
static void deactivateTimer(void){
  gptStop(&GPTD3);
  return;
}

//Initiate SCCB start condition -- requires the timer
static void startCondition(void){
  chSysLockFromIsr();

  //Pull SDA SCL High
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  //Wait
  gptPolledDelay(&GPTD3,CLK_DELAY);
  //Data Low
  palWritePad(CAM_CTL_PORT, CAM_SDA, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  //Clock Low
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);

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

  gptPolledDelay(&GPTD3,CLK_DELAY);
  //SCL High
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  //SDA High
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);

  chSysLockFromIsr();
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
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x80)>>7);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 6
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x40)>>6);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 5
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x20)>>5);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 4
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x10)>>4);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  
  //Bit 3
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x08)>>3);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
 
  //Bit 2
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x04)>>2);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 1
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x02)>>1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);

  //Bit 0
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  palWritePad(CAM_CTL_PORT, CAM_SDA, (byte & 0x01) );
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  

  //9th Don't Care bit
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  //Deassert don't care
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_INPUT );
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  
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
  
  gptPolledDelay(&GPTD3,CLK_DELAY);

  //MSB Bit 7
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<7;

  //Bit 6
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<6;

  //Bit 5
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<5;

  //Bit 4
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<4;

  //Bit 3
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<3;

  //Bit 2
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<2;

  //Bit 1
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) )<<1;

  //Bit 0
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
  byte |= ( palReadPad(CAM_CTL_PORT, CAM_SDA) );

  //9th Don't Care Byte
  palWritePad(CAM_CTL_PORT, CAM_SCL, 0);
  //Dive to 1 per datasheet
  palSetPadMode(CAM_CTL_PORT, CAM_SDA, PAL_MODE_OUTPUT_PUSHPULL |
                                       PAL_STM32_OSPEED_LOWEST );
  palWritePad(CAM_CTL_PORT, CAM_SDA, 1);  
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palWritePad(CAM_CTL_PORT, CAM_SCL, 1);
  gptPolledDelay(&GPTD3,CLK_DELAY);
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
  palSetPad(FIFO_CTL_PORT, FIFO_WEN);
  palSetPadMode(FIFO_CTL_PORT, FIFO_RRST, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(FIFO_CTL_PORT, FIFO_RRST);
  palSetPadMode(FIFO_CTL_PORT, FIFO_OE, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(FIFO_CTL_PORT, FIFO_OE);
  palSetPadMode(FIFO_CTL_PORT, FIFO_RCLK, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(FIFO_CTL_PORT, FIFO_RCLK);
  return;
}

//Check camera sanity
msg_t checkCameraSanity(void){
  uint8_t rxbyte;
  uint8_t tmp;
  //Verify Programming
  for( tmp = 0; tmp < CONFIG_PAIRS; tmp++ ){
    rxbyte = cameraReadCycle( cam_config[tmp][0] );
    chprintf((BaseChannel *)&SD1, "ADDR: %x VAL: %x\r\n", cam_config[tmp][0], rxbyte );
    if( rxbyte != cam_config[tmp][1] )
      return -1;
  }

  return RDY_OK;
}


//Configure the camera
msg_t configureCam(void){
  uint8_t tmp;
  idleState();
  //Poweron the camera
  wakeupCam();
  
  //Configure the camera
  for( tmp = 0; tmp < CONFIG_PAIRS; tmp++ )
    cameraWriteCycle( cam_config[tmp][0], cam_config[tmp][1] );

  tmp = checkCameraSanity();
  powerdownCam();
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
  uint16_t vlower = segment; //segment*8;
  uint16_t vupper = segment; //vlower+7;

  uint8_t vstrt = vlower>>2;
  uint8_t vstop = vupper>>2;
  uint8_t vref = 0x00;
  vref |= (vlower & 0x03);
  vref |= (vupper & 0x03)<<2;

  //Write our window configuration to the control registers
  cameraWriteCycle( CAM_VREF, vref );
  cameraWriteCycle( CAM_VSTART, vstrt );
  cameraWriteCycle( CAM_VSTOP, vstop );

  //Verify write operations were successful
  vlower = cameraReadCycle( CAM_VREF );
  if( vlower != vref )
    return -1;

  vlower = cameraReadCycle( CAM_VSTART );
  if( vlower != vstrt )
    return -1;

  vlower = cameraReadCycle( CAM_VSTOP );
  if( vlower != vstop )
    return -1;

  return RDY_OK;
}
 

void fifoGrabBytes( uint8_t *buf, uint32_t n ){

  uint8_t dataByte = 0;
  while( n > 0 ){
    dataByte = 0;
    palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay(&GPTD3,CLK_DELAY);
    palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay(&GPTD3,CLK_DELAY);
    //TODO: Final version turn this into necessary code
    dataByte |= (palReadPort( FIFO_DATA_PORT ) & 0x0F);
    dataByte |= (palReadPort( FIFO_DATA_PORT ) & 0x3B0)>>2;
    *buf = dataByte;
    buf++;
    n--;
  }
}

static void resetReadPointer(void){
  palClearPad( FIFO_CTL_PORT, FIFO_RRST );
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palClearPad( FIFO_CTL_PORT,FIFO_RCLK );
  gptPolledDelay(&GPTD3,CLK_DELAY);
  palSetPad( FIFO_CTL_PORT,FIFO_RCLK );
}


msg_t cameraControlThread(void* arg){
  chprintf((BaseChannel *)&SD1, "Beginning camera control thread\r\n");
  setupSCCB();
  setupCamPort();

  uint8_t segmentNumber = 0x00;
  while(TRUE){ 
    chThdSleepMilliseconds(2000);
    prepareTimer();
    msg_t success = configureCam();
    if( success == RDY_OK ){
       chprintf((BaseChannel *)&SD1, "Config TX OK\r\n");
    }else{
      chprintf((BaseChannel *)&SD1, "Unknown state returned\r\n");
    }
    for( segmentNumber; segmentNumber < 480; segmentNumber++ ){
      success = setupSegment( segmentNumber );
      palClearPad( FIFO_CTL_PORT, FIFO_OE );
      resetReadPointer();
      palClearPad( FIFO_CTL_PORT, FIFO_WEN );
      chSysLockFromIsr();
      while( !palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      while( palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      while( !palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
      palSetPad( FIFO_CTL_PORT, FIFO_WEN );
      chSysUnlockFromIsr();
      fifoGrabBytes( pixelData, 640*2 );
      chprintf((BaseChannel *)&SD1, "\r\nSegment %d Next Line:\r\n)", segmentNumber );
      sdWrite( &SD1, pixelData, 640*2 );
    }
    deactivateTimer(); 
  }
}
