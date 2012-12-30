/*uart_iface.h
 *
 *
 *This is the definition of the uart communication worker thread
 *
 *It will handle all of our interfacing with the IHU at 19.2 kbaud
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

#ifndef __UART_IFACE_H__
#include "ch.h"
#include "hal.h"

#define __UART_IFACE_H__


extern WORKING_AREA(waUART_Thread, 128);
msg_t UART_Thread(void* arg);

#define UART_COMM_IDLE 0
#define UART_COMM_READ_COMMAND 1
#define UART_COMM_SEND_DATA 2


#endif
