/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#include "ch.h"
#include "hal.h"
#include "uart_iface.h"
#include "camera_iface.h"
#include "spi_flash.h"
#include "chprintf.h"

/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED3);       /* Orange.  */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED3);     /* Orange.  */
    chThdSleepMilliseconds(500);
  }
  return 0;
}

WORKING_AREA(waUART_Thread, 256);
WORKING_AREA(waCamera_Thread, 10240+128);
//WORKING_AREA(waUART_Thread, 256);

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

  /*
   * Creates the example thread.
   */
  //chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
 
  /*
   * Creates the Dummy Camera Thread.
   */
  chThdCreateStatic(waCamera_Thread, sizeof(waCamera_Thread), NORMALPRIO, Camera_Thread, NULL);
  
  /*
   * Creates the Dummy UART Thread.
   */
  chThdCreateStatic(waUART_Thread, sizeof(waUART_Thread), HIGHPRIO, UART_Thread, NULL);
  
  /*
   * Create our UART thread
   */
  //chThdCreateStatic(waUART_Thread, sizeof(waUART_Thread), NORMALPRIO, UART_Thread,NULL);

  //configureSPIFlash();
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */

  /* TODO
   *
   * This is where main() will listen for events from uart_iface and foward them to where they need to go
   */
  EventListener main_events_li;
  
  const eventmask_t myEvents =  WAKE_CAMERA_THREAD; 

  chEvtRegisterMask( &(SD3.event), &main_events_li, myEvents );

  //Test Case for sending an event to wake up the camera thread
/*  while (TRUE) {
    chprintf((BaseChannel *)&SD3, "Inside main() thread\r\n");
    chThdSleepMilliseconds(5000); 
    chSchReadyI(waCamera_Thread);
  }
*/
}
