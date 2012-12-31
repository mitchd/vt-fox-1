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
#define __UART_IFACE_H__


#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#ifdef RELEASE_VERSION

#define COMMAND_SIZE 1

#else

#include "strings.h"
#define UART_SHOWMENU 3
#define UART_RX_RGB 4
#define UART_TX_RGB 5
#define UART_WRITE_SPI_BYTE 6
#define UART_READ_SPI_BYTE 7
#define COMMAND_SIZE 1

#endif



//Define LOCAL_ECHO to echo characters sent through UART (debugging stuffs)
#define LOCAL_ECHO


//Define RELEASE_VERSION to use the release functionality (undef'ing this
//allows you to retain the command parsing code inside an ifdef while working
//on development code
//#define RELEASE_VERSION

extern WORKING_AREA(waUART_Thread, 128);
msg_t UART_Thread(void* arg);


#define UART_COMM_IDLE 0
#define UART_COMM_READ_COMMAND 1
#define UART_COMM_ERROR -1
#define UART_COMM_SEND_DATA 2


#endif
