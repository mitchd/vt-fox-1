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
#include "spi_flash.h"

#ifdef RELEASE_VERSION

#define COMMAND_SIZE 1

#else

#define UART_SHOWMENU 3
#define UART_RX_RGB 4
#define UART_TX_RGB 5
#define UART_WRITE_SPI_BYTE 6
#define UART_READ_SPI_BYTE 7
#define UART_WAKE_CAMERA 8
#define COMMAND_SIZE 1

//Events mask bits for events from uart_iface to main
#define WAKE_CAMERA_THREAD 1024 

#endif



//Define LOCAL_ECHO to echo characters sent through UART (debugging stuffs)
#define LOCAL_ECHO


//Define RELEASE_VERSION to use the release functionality (undef'ing this
//allows you to retain the command parsing code inside an ifdef while working
//on development code
//#define RELEASE_VERSION

extern WORKING_AREA(waUART_Thread, 256);
/*
 * UART_Thread
 *
 * This is the thread that handle all the UART communications for the board
 *
 * It's primary tasks are:
 *
 *      1) Receive and parse commands from the IHU
 *      2) Signal the main thread to initiate a camera capture
 *      3) Send requested data to the IHU
 *              A) Telemetry (if implemented)
 *              B) jpeg data
 *
 * As these additional feature are implemented *arg will begin to develop and
 * documentation about the argument parameters will be provided.
 *
 *
 * Key internal variables in this function:
 *
 * serial_events:
 *      This is our event listener.  It is registered to the serial driver.  As
 *      our error-handling code becomes more sophisticated, we can register all
 *      possible serial events.
 *
 * myUART_events:
 *      This is the event mask that gets registered to the serial driver.  Event
 *      names are fairly self-explanatory
 *
 *COMMAND_SIZE (defined in uart_iface.h):
 *      We expect commands from the IHU to be a certain length (1-4 bytes,
 *      depending on the error correction we want in the actual commands or
 *      whatev).
 *
 *readSize, writeSize:
 *      Track the number of bytes read or written to the serial device for error
 *      handling.
 *
 *read_buffer[COMMAND_SIZE]:
 *      We only expect to receive COMMAND_SIZE chunks of data from the IHU.
 *      With the exception of some debug/development-specific code (e.g. to
 *      transfer data from camera to computer, or rgb data from computer to flash
 *      We will also only give COMMAND_SIZE chunks to the uC
 *
 */
msg_t UART_Thread(void* arg);


#define UART_COMM_IDLE 0
#define UART_COMM_READ_COMMAND 1
#define UART_COMM_ERROR -1
#define UART_COMM_SEND_DATA 2


#endif
