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

  palSetMode(GPIOB, 8, PAL_MODE_ALTERNATE(4) |
                       PAL_MODE_OUTPUT_OPENDRAIN );
  palSetMode(GPIOB, 9, PAL_MODE_ALTERNATE(4) |
                       PAL_MODE_OUTPUT_OPENDRAIN );
}

//Configure the camera pads
void setupCamPort(void){
  palSetMode(CAM_PORT, CAM_RESET, PAL_MODE_OUTPUT_PUSHPULL);
  palSetMode(CAM_PORT, CAM_PWDN, PAL_MODE_OUTPUT_PUSHPULL);
  palSetMode(CAM_PORT2, CAM_VSYNC_OUT, PAL_MODE_INPUT);
  palSetMode(CAM_PORT2, CAM_HREF_OUT, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D0, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D1, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D2, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D3, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D4, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D5, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D6, PAL_MODE_INPUT);
  palSetMode(FIFO_DATA_PORT, FIFO_D7, PAL_MODE_INPUT);
  palSetMode(FIFO_CTL_PORT, FIFO_WEN, PAL_MODE_OUTPUT_PUSHPULL);
  palSetMode(FIFO_CTL_PORT, FIFO_RRST, PAL_MODE_OUTPUT_PUSHPULL);
  palSetMode(FIFO_CTL_PORT, FIFO_OE, PAL_MODE_OUTPUT_PUSHPULL);
  palSetMode(FIFO_CTL_PORT, FIFO_RCLK, PAL_MODE_OUTPUT_PUSHPULL);
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

msg_t Camera_Thread(void* arg) {
  (void)arg;
  
  /* TODO 
   * migrate the wake event into the camera_events and myCamera_events
   */

  //Pulled from uart_iface.c, is used for serial comms
  //Register this as an event listener for events from SD3
  //EventListener serial_events;
  //Events received from the serial device
  //const eventmask_t myCamera_events = IO_INPUT_AVAILABLE | 
  //                                 SD_OVERRUN_ERROR | 
  //                                 IO_OUTPUT_EMPTY;
  //chEvtRegisterMask( &(SD3.event), &serial_events, myUART_events );

  //Make our read buffer the same size as the buffer used with the serial device 
  uint8_t read_buffer[COMMAND_SIZE];
  //
  //Keep track of our current communication state with myUART_state
  //This should be transformed to a global, signal, something....
  //uint8_t myUART_state = UART_COMM_IDLE;
  //recorded events
  eventmask_t events;

  //NOTE: this is an older version of configureCam and hangs the code, I am using it as a placeholder
  //Powers on the camera, configures it, powers down the camera
  //configureCam();             
  chprintf((BaseChannel*)&SD3, "(CT) Camera configured and powerd off\n");

  //This is a test, the camera sleeps and waits for the event to wake it up
  while (TRUE) {
      chprintf((BaseChannel*)&SD3, "(CT) Time for Camera Thread to Sleep\n");
      chSchGoSleepS(THD_STATE_SUSPENDED);

      chprintf((BaseChannel*)&SD3, "(CT) Camera Thread has awoken\n");
  }


}

