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



/*uart_iface.h
 *
 *This is the definition of the uart communication worker thread
 *
 *It will handle all of our interfacing with the IHU at 38.4 kbaud
 *
 */

#ifndef __UART_IFACE_H__
#define __UART_IFACE_H__


#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "spi_flash.h"

#define RELEASE_VERSION

#define MESSAGE_CMD_SIZE 6
#define MESSAGE_DATA_SIZE 512
#define MESSAGE_CMD_REPLY_SIZE 6
#define MESSAGE_DATA_REPLY_SIZE 8

#define MESSAGE_VERSION 120 // 1.10
#define SOFTWARE_BUILD  100 // 1.00

#define UART_DBG_PRINT
#define UART_THREAD_SIZE 2048 
extern WORKING_AREA(waUART_Thread, UART_THREAD_SIZE);
msg_t UART_Thread(void* arg);
#define IHU_UART (BaseSequentialStream*)&SD3
#define IHU_UART_DEV	&SD3
#define DBG_UART (BaseSequentialStream*)&SD1
#define DBG_UART_DEV	&SD1

#define UART_COMM_IDLE         0
#define UART_COMM_READ_COMMAND 1
#define UART_COMM_ERROR        -1
#define UART_COMM_SEND_DATA    2

#define CMD_READY   (uint8_t)'R'
#define CMD_TRANS   (uint8_t)'T'
#define RESP_NREADY (uint8_t)'N'
#define RESP_READY  (uint8_t)'Y'
#define RESP_FAILED (uint8_t)'F'

//Signalling variables
extern uint8_t cameraHealth;
#define CAMERA_HEALTHY 1
#define CAMERA_FAILED 0
extern uint8_t cameraThreadDone;
#define CAMERA_THREAD_BUSY 0
#define CAMERA_THREAD_DONE 1

#endif
