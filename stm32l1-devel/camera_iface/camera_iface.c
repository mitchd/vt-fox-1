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
#define CONFIG_PAIRS 15
//VGA 565
static const uint8_t cam_config[CONFIG_PAIRS][2] = {
  {CAM_CLKRC, 0x80},
  {CAM_COM3, 0x00},
  {CAM_COM7, 0x04},
  {CAM_COM10, 0x02},
  {CAM_RGB444, 0x00},
  {CAM_COM11, 0x0A},
  //{CAM_COM14, 0x18},
  {CAM_COM15, 0xD0},
  {CAM_SCALING_DCWCTR, 0x00},
  {CAM_SCALING_PCK_DIV, 0xF0},
  //{CAM_TSLB, 0x04 },
  {CAM_SCALING_PCK_DELAY, 0x00 },
  {CAM_SCALING_XSC, 0x20 },
  {CAM_SCALING_YSC, 0x20 },
  {CAM_HSTOP, 0x4F},
  {CAM_HSTART, 0x00},
  {CAM_HREF, 0xB8},
/*  {CAM_VSTART, 0x01},
  {CAM_VSTOP, 0x3C},
  {CAM_VREF, 0x0C}
/*  	{0x3a, 0x04},//
	{0x40, 0x10},
	{0x12, 0x14},
	{0x32, 0x80},
	{0x17, 0x16},
	{0x18, 0x04},
	{0x19, 0x02},
	{0x1a, 0x7b},
	{0x03, 0x06},
	{0x0c, 0x0c},
    {0x15, 0x02},
	{0x3e, 0x00},
	{0x70, 0x00},
	{0x71, 0x01},
	{0x72, 0x11},
	{0x73, 0x09},
	{0xa2, 0x02},
	{0x11, 0x00},
	{0x7a, 0x20},
	{0x7b, 0x1c},
	{0x7c, 0x28},
	{0x7d, 0x3c},
	{0x7e, 0x55},
	{0x7f, 0x68},
	{0x80, 0x76},
	{0x81, 0x80},
	{0x82, 0x88},
	{0x83, 0x8f},
	{0x84, 0x96},
	{0x85, 0xa3},
	{0x86, 0xaf},
	{0x87, 0xc4},
	{0x88, 0xd7},
	{0x89, 0xe8},
	{0x0d, 0x00},
	{0x14, 0x20},
	{0xa5, 0x05},
	{0xab, 0x07},
	{0x24, 0x75},
	{0x25, 0x63},
	{0x26, 0xA5},
	{0x9f, 0x78},
	{0xa0, 0x68},
	{0xa1, 0x03},
	{0xa6, 0xdf},
	{0xa7, 0xdf},
	{0xa8, 0xf0},
	{0xa9, 0x90},
	{0xaa, 0x94},
	{0x13, 0xe5},
	{0x0e, 0x61},
	{0x0f, 0x4b},
	{0x16, 0x02},
	{0x1e, 0x37}, 
	{0x21, 0x02},
	{0x22, 0x91},
	{0x29, 0x07},
	{0x33, 0x0b},
	{0x35, 0x0b},
	{0x37, 0x1d},
	{0x38, 0x71},
	{0x39, 0x2a},
	{0x3c, 0x78},
	{0x4d, 0x40},
	{0x4e, 0x20},
	{0x69, 0x5d},
	{0x6b, 0x40},
	{0x74, 0x19},
	{0x8d, 0x4f},
	{0x8e, 0x00},
	{0x8f, 0x00},
	{0x90, 0x00},
	{0x91, 0x00},
	{0x92, 0x00},
	{0x96, 0x00},
	{0x9a, 0x80},
	{0xb0, 0x84},
	{0xb1, 0x0c},
	{0xb2, 0x0e},
	{0xb3, 0x82},
	{0xb8, 0x0a},
	{0x43, 0x14},
	{0x44, 0xf0},
	{0x45, 0x34},
	{0x46, 0x58},
	{0x47, 0x28},
	{0x48, 0x3a},
	{0x59, 0x88},
	{0x5a, 0x88},
	{0x5b, 0x44},
	{0x5c, 0x67},
	{0x5d, 0x49},
	{0x5e, 0x0e},
	{0x64, 0x04},
	{0x65, 0x20},
	{0x66, 0x05},
	{0x94, 0x04},
	{0x95, 0x08},
	{0x6c, 0x0a},
	{0x6d, 0x55},
	{0x4f, 0x80},
	{0x50, 0x80},
	{0x51, 0x00},
	{0x52, 0x22},
	{0x53, 0x5e},
	{0x54, 0x80},
	{0x6e, 0x11},
	{0x6f, 0x9f},
    {0x55, 0x00}, 
    {0x56, 0x40},
    {0x57, 0x80}, */	      
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
  palClearPad(FIFO_CTL_PORT, FIFO_WEN);
  palSetPadMode(FIFO_CTL_PORT, FIFO_RRST, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(FIFO_CTL_PORT, FIFO_RRST);
  palSetPadMode(FIFO_CTL_PORT, FIFO_OE, PAL_MODE_OUTPUT_PUSHPULL);
  palClearPad(FIFO_CTL_PORT, FIFO_OE);
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
    //chprintf((BaseChannel *)&SD1, "ADDR: %x VAL: %x\r\n", cam_config[tmp][0], rxbyte );
    if( rxbyte != cam_config[tmp][1] ){
      chprintf((BaseChannel *)&SD3, "ADDR: %x ASSIGNED: %x ACTUAL: %x\r\n",cam_config[tmp][0],cam_config[tmp][1],rxbyte);
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
//  wakeupCam();
  //Set Default Values
  cameraWriteCycle( CAM_COM7, 0x80 );
  cameraWriteCycle( CAM_CLKRC, 0x80 );
  //Configure the camera
  for( tmp = 0; tmp < CONFIG_PAIRS; tmp++ )
    cameraWriteCycle( cam_config[tmp][0], cam_config[tmp][1] );

  tmp = checkCameraSanity();
        cameraWriteCycle(0x4f,0x80);
        cameraWriteCycle(0x50,0x80);
        cameraWriteCycle(0x51,0x00);
        cameraWriteCycle(0x52,0x22);
        cameraWriteCycle(0x53,0x5e);
        cameraWriteCycle(0x54,0x80);
        cameraWriteCycle(0x56,0x40);
        cameraWriteCycle(0x58,0x9e);
        cameraWriteCycle(0x59,0x88);
        cameraWriteCycle(0x5a,0x88);
        cameraWriteCycle(0x5b,0x44);
        cameraWriteCycle(0x5c,0x67);
        cameraWriteCycle(0x5d,0x49);
        cameraWriteCycle(0x5e,0x0e);
        cameraWriteCycle(0x69,0x00);
        cameraWriteCycle(0x6a,0x40);
        cameraWriteCycle(0x6b,0x0a);
        cameraWriteCycle(0x6c,0x0a);
        cameraWriteCycle(0x6d,0x55);
        cameraWriteCycle(0x6e,0x11);
        cameraWriteCycle(0x6f,0x9f);

        cameraWriteCycle(0xb0,0x84);
//  powerdownCam();
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
    vstrt = 0x00;  //Row 0 to 239
    vstop = 0x3B;
    vref = 0x0C;
  }else{ //"Lower" Half of image
    vstrt = 0x3C;  //Row 240 to 479
    vstop = 0x77;
    vref = 0x0C;
  }

  //Write our window configuration to the control registers
  cameraWriteCycle( CAM_VREF, vref );
  cameraWriteCycle( CAM_VSTART, vstrt );
  cameraWriteCycle( CAM_VSTOP, vstop );

  //Verify write operations were successful
  uint8_t tmp;
  tmp = cameraReadCycle( CAM_VREF );
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
    //gptPolledDelay( &GPTD3, 10 );
    palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
    //TODO: Final version turn this into necessary code
    dataByte = (palReadPort( FIFO_DATA_PORT ) & 0x0F);
    dataByte |= (palReadPort( FIFO_DATA_PORT ) & 0x3C0)>>2;
    buf[i] = dataByte;
    //gptPolledDelay( &GPTD3, 10 );
  }
}

static void resetReadPointer(void){
  // ~RRST
  palClearPad( FIFO_CTL_PORT, FIFO_RRST );
  //gptPolledDelay( &GPTD3, 10 );
  palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
  //gptPolledDelay( &GPTD3, 10 );
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  //gptPolledDelay( &GPTD3, 10 );
  // RRST High
  palSetPad( FIFO_CTL_PORT, FIFO_RRST );
  //gptPolledDelay( &GPTD3, 10 );
  palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
  //gptPolledDelay( &GPTD3, 10 );
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  //gptPolledDelay( &GPTD3, 10 );
}


msg_t cameraControlThread(void* arg){
  prepareTimer();
  setupCamPort();
  palSetPadMode(GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOB, 6, PAL_MODE_OUTPUT_PUSHPULL);
  palClearPad( GPIOB, 7 );
  palClearPad( GPIOB, 6 );

  uint8_t pixelData[IMG_WIDTH*2*8];
  setupSCCB();

  uint8_t segmentNumber = 0x00;
  msg_t success = configureCam();
  if( success == RDY_OK ){  
    //Ensure WEN is disabled
    palClearPad( FIFO_CTL_PORT, FIFO_WEN );
    jpeg_init();
    for( segmentNumber = 0; segmentNumber < 2; segmentNumber++ ){
      if( setupSegment( segmentNumber ) != RDY_OK ){
        segmentNumber--;
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
        uint8_t bulk_reads;
        for( bulk_reads=0; bulk_reads < IMG_HEIGHT/8/2; bulk_reads++ ){
          palSetPad( GPIOB, 6 );
          fifoGrabBytes( pixelData, IMG_WIDTH*2*8 );
          convert_rows( pixelData );
          palClearPad( GPIOB, 6 );
        }
      }
    }
    jpeg_close();
  }else{
    chprintf((BaseChannel *)&SD3,"SETUP FAILED\r\n");
  }
  palSetPad( GPIOB, 7 );
  while(TRUE); 
}
