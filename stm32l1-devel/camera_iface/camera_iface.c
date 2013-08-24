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
#define CONFIG_PAIRS 50
//
//30 FPS VGA YUV Mode
//
#define HSTART (158) //Stole these magic numbers from the kernel driver
#define HSTOP (14)
static const uint8_t cam_config[CONFIG_PAIRS][2] = {
  {CAM_CLKRC, 0x01},
  {CAM_COM3, 0x00}, //enable scaling
  {CAM_COM7, 0x00}, //YUV
  {CAM_COM14, 0x00},
  {CAM_COM10, 0x02},  //VERY IMPORTANT VSYNC INVERT
  {CAM_RGB444, 0x00}, //Disable RGB444
  {CAM_COM15, 0xC0}, //YUV full range
  {CAM_SCALING_DCWCTR, 0x11},
  {CAM_SCALING_PCK_DIV, 0xF0},
  {CAM_SCALING_PCK_DELAY, 0x00 },
  {CAM_SCALING_XSC, 0x3A },
  {CAM_SCALING_YSC, 0x4A },
  {CAM_MVFP, 0x31}, //Flip horizontally and vertically
  //Stole some magic from the linux driver
  {CAM_HREF, (HSTART & 0x7) | ((HSTOP & 0x7)<<3) | 0xC0},
  {CAM_HSTART, HSTART>>3 & 0xFF}, 
  {CAM_HSTOP, HSTOP>>3 & 0xFF},
  //Gamma Curve
  { 0x7a, 0x20 },         { 0x7b, 0x10 },
  { 0x7c, 0x1e },         { 0x7d, 0x35 },
  { 0x7e, 0x5a },         { 0x7f, 0x69 },
  { 0x80, 0x76 },         { 0x81, 0x80 },
  { 0x82, 0x88 },         { 0x83, 0x8f },
  { 0x84, 0x96 },         { 0x85, 0xa3 },
  { 0x86, 0xaf },         { 0x87, 0xc4 },
  { 0x88, 0xd7 },         { 0x89, 0xe8 },
  //AGC and AEC
  { CAM_GAIN, 0 },        { CAM_AECH, 0 },
  { CAM_COM4, 0x40 }, /* magic reserved bit */
  { CAM_COM9, 0x18 }, /* 4x gain + magic rsvd bit */
  { CAM_BD50MAX, 0x05 },  { CAM_BD60MAX, 0x07 },
  { CAM_AEW, 0x95 },      { CAM_AEB, 0x33 },
  { CAM_VPT, 0xe3 },      { CAM_HAECC1, 0x78 },
  { CAM_HAECC2, 0x68 },   { 0xa1, 0x03 }, /* magic */
  { CAM_HAECC3, 0xd8 },   { CAM_HAECC4, 0xd8 },
  { CAM_HAECC5, 0xf0 },   { CAM_HAECC6, 0x90 },
  { CAM_HAECC7, 0x94 },
  { CAM_COM8, 0x8F } //Turn on AGC, AWB, AEC
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
  gptPolledDelay(&GPTD3,PWR_DELAY);
  //Set Default Values
  cameraWriteCycle( CAM_COM7, 0x80 );
  cameraWriteCycle( CAM_CLKRC, 0x80 );
  cameraWriteCycle( CAM_COM8, 0x00 ); //Turn off AGC, AWB, AEC for config
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
#define SEG0_VSTART 10
#define SEG0_VSTOP 250
#define SEG1_VSTART 250
#define SEG1_VSTOP 490
msg_t setupSegment( uint8_t segment )
{ 
  
  uint8_t vstrt, vstop, vref;
  if( segment == 0 ){ //"Upper" Half of image
    vstrt = SEG0_VSTART >> 2;  
    vstop = SEG0_VSTOP >> 2;
    vref = 0x00 | (SEG0_VSTART & 0x3) | ((SEG0_VSTOP & 0x3) <<2);
  }else{ //"Lower" Half of image
    vstrt = SEG1_VSTART >> 2;  
    vstop = SEG1_VSTOP >> 2;
    vref = 0x00 | (SEG1_VSTART & 0x3) | ((SEG1_VSTOP & 0x3) <<2);
  }

  //Write our window configuration to the control registers
  cameraWriteCycle( CAM_VREF, vref );
  cameraWriteCycle( CAM_VSTART, vstrt );
  cameraWriteCycle( CAM_VSTOP, vstop );

  //Verify write operations were successful
  uint8_t tmp;
  tmp = cameraReadCycle( CAM_VREF );
  //chprintf(IHU_UART,"%x\t%x\r\n",vref,tmp);
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
 

void fifoGrabBytes( uint8_t *buf, uint32_t n, uint8_t ignore ){
  uint32_t i = 0;
  for( i=0; i<ignore; i++ ){
    palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay( &GPTD3, FIFO_DELAY );
    palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay( &GPTD3, FIFO_DELAY );
  }
  for( i=0; i<n; i++ ){
    palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay( &GPTD3, FIFO_DELAY );
    palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
    gptPolledDelay( &GPTD3, FIFO_DELAY );
    buf[i] = (palReadPort(FIFO_DATA_PORT) & 0xFF);
  }
}

static void resetReadPointer(void){
  // ~RRST
  palClearPad( FIFO_CTL_PORT, FIFO_RRST );
  gptPolledDelay( &GPTD3, FIFO_DELAY );
  palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD3, FIFO_DELAY );
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD3, FIFO_DELAY );
  // RRST High
  palSetPad( FIFO_CTL_PORT, FIFO_RRST );
  gptPolledDelay( &GPTD3, FIFO_DELAY );
  palClearPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD3, FIFO_DELAY );
  palSetPad( FIFO_CTL_PORT, FIFO_RCLK );
  gptPolledDelay( &GPTD3, FIFO_DELAY );
}


msg_t cameraControlThread(void* arg){
  const uint16_t IMG_WIDTH = 640;
  const uint16_t READ_SIZE = IMG_WIDTH*2*8;
  const uint16_t NUM_READS = 30;
  chThdSetPriority( HIGHPRIO );
  //chprintf(IHU_UART,"Setup Timer\r\n");
  prepareTimer();
  //chprintf(IHU_UART,"Setup Cam Port\r\n");
  setupCamPort();

  uint8_t pixelData[IMG_WIDTH*2*8];
  //chprintf(IHU_UART,"Setup SCCB Interface\r\n");
  setupSCCB();

  //chprintf(IHU_UART,"Configure Camera\r\n");
  msg_t success = configureCam();
  uint8_t segment;
  jpeg_init();
  for( segment = 0; segment < 2; segment++ )
  {
    //Wakeup Camera
    wakeupCam();
    //Wait 1ms per datasheet
    gptPolledDelay( &GPTD3, PWR_DELAY );
    if( success == RDY_OK ){  
      //Ensure WEN is disabled
      palClearPad( FIFO_CTL_PORT, FIFO_WEN );
      if( setupSegment( segment ) != RDY_OK ){
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
        //VSYNC Low = Beginning of Frame - WRST is pulled low to reset write reg
        while( !palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
        //Wait until VSYNC goes low again
        while( palReadPad( CAM_PORT2, CAM_VSYNC_OUT ) );
        //Disable WEN
        palClearPad( FIFO_CTL_PORT, FIFO_WEN );
        //Poweroff cam
        //powerdownCam();
        uint8_t bulk_reads;
        if( segment == 0 )
	  fifoGrabBytes( pixelData, 0, 1 );
        for( bulk_reads=0; bulk_reads < NUM_READS; bulk_reads++ ){
          fifoGrabBytes( pixelData, READ_SIZE, 0 );
          //Process the line
          encode_line_yuv( pixelData, bulk_reads + NUM_READS*segment );
          //sdWrite( IHU_UART_DEV, &pixelData[0], READ_SIZE );
        }
      }
    }
  }
  jpeg_close();
#ifndef RELEASE
  uint32_t image_end = jpeg_addr_ptr();
  uint32_t image_start = IMAGE_DATA_START;
  uint32_t num_steps = (image_end-image_start)/SERIAL_BUFFERS_SIZE;
  uint32_t image_ptr = image_start;
  uint32_t step = 0;
  for(; step < num_steps; step++)
  {
    flashReadBytes( image_ptr, pixelData, SERIAL_BUFFERS_SIZE );
    sdWrite( IHU_UART_DEV, pixelData, SERIAL_BUFFERS_SIZE );
    image_ptr += SERIAL_BUFFERS_SIZE;
  }
  uint32_t remainder = image_end - image_ptr;
  flashReadBytes( image_ptr, pixelData, remainder );
  sdWrite( IHU_UART_DEV, pixelData, remainder );
#endif //~RELEASE
  while(TRUE); 
}
