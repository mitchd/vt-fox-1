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
#include "ch.h"
#include "hal.h"
#include "stdlib.h"
#include "uart_iface.h"
#include "spi_flash.h"
#include "camera_iface.h"
#include "chprintf.h"

//WORKING_AREA(waUART_Thread, 512);
//WORKING_AREA(waCamera_Thread, 10240+1024);

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 3.
   * PB10(TX) and PB11(RX) are routed to USART3.
   */
  sdStart(&SD3, NULL);
  palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));

  /* Activate serial driver 4 for debug
   *  PA9 (TX) and PA10(RX)
   *
   */
  sdStart(&SD1,NULL);
  palSetPadMode(GPIOA, 9 , PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));
  /*
   * Create our UART thread
   */
  //chThdCreateStatic(waUART_Thread, sizeof(waUART_Thread), NORMALPRIO,
  //                  UART_Thread,NULL);

  configureSPIFlash();

  //chThdCreateStatic(waCamera_Thread, sizeof(waCamera_Thread), HIGHPRIO,
  //                  cameraControlThread,NULL);
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */
  system_data writeData;
  system_data readData;
  
  uint8_t i = 0;
  for( i=0; i<80; i++ ){
    writeData.line[i].line_num = i;
    writeData.line[i].start_addr = ((uint32_t)i*1000)+IMAGE_DATA_START;
    writeData.line[i].end_addr = ((uint32_t)(i+1)*1000)-1+IMAGE_DATA_START;
    writeData.line[i].chksum = 80-i;
  }
  line_data lineR, lineW;
  while (TRUE) {
    //Write data to SPI flash
    flashWriteBytes( SYSTEM_DATA_ADDR, (uint8_t*)&writeData, sizeof(writeData) );
    //Wait three seconds
    chThdSleepMilliseconds(3000);
    //Read data from SPI flash
    flashReadBytes( SYSTEM_DATA_ADDR, (uint8_t*)&readData, sizeof(readData) );
    for( i=0; i<80; i++ ){
        chThdSleepMilliseconds(500);
        lineW = writeData.line[i];
        lineR = readData.line[i];
        chprintf(IHU_UART,"Saved Data:\r\n");
        chprintf(IHU_UART,"Line %d %X %X %d\r\n\n\n", lineW.line_num, lineW.start_addr, lineW.end_addr, lineW.chksum);
        chprintf(IHU_UART,"Read Data:\r\n");
        chprintf(IHU_UART,"Line %d %X %X %d\r\n\n\n", lineR.line_num, lineR.start_addr, lineR.end_addr, lineR.chksum);
        if (lineW.start_addr == lineR.start_addr)
            if (lineW.end_addr == lineR.end_addr)
                if (lineW.chksum == lineR.chksum)
                    chprintf( IHU_UART, "Data check SUCCESS\r\n" );
    }
  }
}
