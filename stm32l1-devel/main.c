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
Kevin Burns, KJ4SYL, kevinpd@vt.edu
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
#include "uart_iface.h"
#include "spi_flash.h"
#include "camera_iface.h"
#include "chprintf.h"

WORKING_AREA(waUART_Thread, 512);
WORKING_AREA(waCamera_Thread, 5120+128);

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

  /* Activate serial driver 1 for debug
   *  PA9 (TX) and PA10(RX)
   *
   *  TODO: On final version, this is UART4 and PC10, PC11
   */
  sdStart(&SD1,NULL);
  palSetPadMode(GPIOA, 9 , PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));
  /*
   * Create our UART thread
   */
  chThdCreateStatic(waUART_Thread, sizeof(waUART_Thread), NORMALPRIO,
                    UART_Thread,NULL);

  configureSPIFlash();

  chThdCreateStatic(waCamera_Thread, sizeof(waCamera_Thread), NORMALPRIO,
                    cameraControlThread,NULL);
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */
  while (TRUE) {
//    chprintf((BaseChannel *)&SD1, "Inside main() thread\r\n");
    chThdSleepMilliseconds(500);
  }
}
