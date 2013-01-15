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
#define COMMAND_SIZE 1

#endif



//Define LOCAL_ECHO to echo characters sent through UART (debugging stuffs)
#define LOCAL_ECHO


//Define RELEASE_VERSION to use the release functionality (undef'ing this
//allows you to retain the command parsing code inside an ifdef while working
//on development code
//#define RELEASE_VERSION

extern WORKING_AREA(waUART_Thread, 512);
msg_t UART_Thread(void* arg);


#define UART_COMM_IDLE 0
#define UART_COMM_READ_COMMAND 1
#define UART_COMM_ERROR -1
#define UART_COMM_SEND_DATA 2


#endif
