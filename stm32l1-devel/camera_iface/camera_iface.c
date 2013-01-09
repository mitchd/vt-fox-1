#include "camera_iface.h"


//I2C#1 Configuration stuff
static const I2CConfig i2cfg1 = {
  OPMODE_I2C,
  400000,
  FAST_DUTY_CYCLE_2,
};

//Setup the I2C controller
void setupI2C(void){
  i2cInit();

  i2cStart(&I2CD1, &i2cfg1);

  palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4) |
                          PAL_MODE_OUTPUT_OPENDRAIN );
  palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4) |
                          PAL_MODE_OUTPUT_OPENDRAIN );
}

//Configure the camera pads
void setupCamPort(void){
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
  msg_value = i2cMasterTransmitTimeout( &I2CD1, CAM_I2C_ADDR,
                                        tx_buf, 18,
                                        rx_buf, 0,
                                        TIME_IMMEDIATE );
  i2cReleaseBus(&I2CD1);

  //Poweroff camera
  powerdownCam();
  return msg_value;
 

}

void wakeupCam(){
  palClearPad(CAM_PORT, CAM_PWDN);
}

void powerdownCam(){
  palSetPad(CAM_PORT, CAM_PWDN);
}

void cameraControlThread(void /*void* arg*/){
  setupCamPort();
  msg_t success = configureCam();
  chprintf((BaseChannel *)&SD1, "Camera config status: %d", success);
  while(TRUE){
    chprintf((BaseChannel *)&SD1, "Inside camera thread\r\n");
    chThdSleepMilliseconds(500);
  }
}
