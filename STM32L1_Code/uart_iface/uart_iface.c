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


#include "uart_iface.h"

/*
 * UART_Thread
 *
 * This is the thread that handle all the UART communications for the board
 *
 * It's primary tasks are:
 *
 *  1) Receive and parse commands from the IHU
 *  2) Send requested data to the IHU
 *      A) Telemetry (if implemented)
 *      B) jpeg data
 *
 * As these additional feature are implemented *arg will begin to develop and
 * documentation about the argument parameters will be provided.
 *
 *
 * Key internal variables in this function:
 *
 * serial_events:
 *  This is our event listener.  It is registered to the serial driver.  As
 *  our error-handling code becomes more sophisticated, we can register all
 *  possible serial events.
 *
 * myUART_events:
 *  This is the event mask that gets registered to the serial driver.  Event
 *  names are fairly self-explanatory
 *
 * COMMAND_SIZE (defined in uart_iface.h):
 *  We expect commands from the IHU to be a certain length (1-6 bytes,
 *  depending on the error correction we want in the actual commands or
 *  whatev).
 *
 * readSize, writeSize:
 *  Track the number of bytes read or written to the serial device for error
 *  handling.
 *
 * read_buffer[COMMAND_SIZE]:
 *  We only expect to receive COMMAND_SIZE chunks of data from the IHU.
 *  With the exception of some debug/development-specific code (e.g. to
 *  transfer data from camera to computer, or rgb data from computer to flash
 *  We will also only give COMMAND_SIZE chunks to the uC
 *
 */

void UART_Reply_Failed(uint8_t *buffer) {
    // Generate and return the message reply if the camera has failed
    buffer[0] = (uint8_t)(MESSAGE_VERSION&0xFF);
    buffer[1] = (uint8_t)(MESSAGE_VERSION>>8);
    buffer[2] = (uint8_t)(SOFTWARE_BUILD&0xFF);
    buffer[3] = (uint8_t)(SOFTWARE_BUILD>>8);
    buffer[4] = RESP_FAILED;
    buffer[5] = RESP_FAILED;
    sdAsynchronousWrite(IHU_UART_DEV, buffer, MESSAGE_CMD_REPLY_SIZE);
}

void UART_Reply_NReady(uint8_t *buffer) {
    // Generate and return the message reply if the camera is not ready
    buffer[0] = (uint8_t)(MESSAGE_VERSION&0xFF);
    buffer[1] = (uint8_t)(MESSAGE_VERSION>>8);
    buffer[2] = (uint8_t)(SOFTWARE_BUILD&0xFF);
    buffer[3] = (uint8_t)(SOFTWARE_BUILD>>8);
    buffer[4] = RESP_NREADY;
    buffer[5] = RESP_NREADY;
    sdAsynchronousWrite(IHU_UART_DEV, buffer, MESSAGE_CMD_REPLY_SIZE);
}

void UART_Reply_Ready(uint8_t *buffer) {
    // Generate and return the message reply if the camera is ready
    buffer[0] = (uint8_t)(MESSAGE_VERSION&0xFF);
    buffer[1] = (uint8_t)(MESSAGE_VERSION>>8);
    buffer[2] = (uint8_t)(SOFTWARE_BUILD&0xFF);
    buffer[3] = (uint8_t)(SOFTWARE_BUILD>>8);
    buffer[4] = RESP_READY;
    buffer[5] = RESP_READY;
    sdAsynchronousWrite(IHU_UART_DEV, buffer, MESSAGE_CMD_REPLY_SIZE);
}

void UART_Reply_Data(uint8_t line_ID, uint16_t length, uint8_t *data, uint8_t *buffer) {
    buffer[0] = (uint8_t)(MESSAGE_VERSION&0xFF);
    buffer[1] = (uint8_t)(MESSAGE_VERSION>>8);
    buffer[2] = (uint8_t)(SOFTWARE_BUILD&0xFF);
    buffer[3] = (uint8_t)(SOFTWARE_BUILD>>8);
    // Line ID is the 6 MSB and length is 10 LSB in the first two bytes
    buffer[4] = length & 0xFF;
    buffer[5] = (line_ID << 2) | ((length & 0x300) >> 8);
    // Copy the data to send to the buffer
    uint16_t i;
    uint16_t chksum = 0;
    for (i = 0; i < length; i++) {
        chksum = chksum + data[i];
    }

    sdAsynchronousWrite(IHU_UART_DEV, buffer, 6);
    sdAsynchronousWrite(IHU_UART_DEV, data, length&0x3FF);
    sdAsynchronousWrite(IHU_UART_DEV, (uint8_t*)&chksum, 2);
}

msg_t UART_Thread(void* arg) {
    (void) arg;

    // Register this as an event listener for events from IHU_UART_DEV
    EventListener serial_events;
    // Events received from the serial device
    const eventmask_t myUART_events = CHN_INPUT_AVAILABLE | 
                                        SD_OVERRUN_ERROR | 
                                        CHN_OUTPUT_EMPTY;
    chEvtRegisterMask( (IHU_UART_DEV.event), &serial_events, myUART_events );

    // Make our read buffer the same size as the buffer used with the serial 
    // device 
    uint8_t read_buffer[MESSAGE_CMD_SIZE] = {0};
    uint8_t write_cmd_buffer[MESSAGE_CMD_REPLY_SIZE] = {0};
    uint8_t i;
    int line = 0;
    uint8_t lineData[1500];
    // Recorded events
    eventmask_t events;

    while (TRUE) {
        // Wait for a serial event
        events = chEvtWaitOne(myUART_events);

        if (events & CHN_INPUT_AVAILABLE) {
            if (!(events & SD_OVERRUN_ERROR)){
              // Blocks until we have read a set number of bytes
              sdRead(IHU_UART_DEV, read_buffer, 2);
              sdRead(IHU_UART_DEV, &read_buffer[2], 2);
              sdRead(IHU_UART_DEV, &read_buffer[4], 2);
            }
            #ifdef UART_DBG_PRINT
            chprintf(DBG_UART, "UART data received: \r\n");
            for (i = 0; i < MESSAGE_CMD_SIZE; i++) {
                chprintf(DBG_UART, "%02X ", read_buffer[i]);
            }
            chprintf(DBG_UART, "\r\n");
            #endif

            // First check for transmission errors
            if (events & SD_OVERRUN_ERROR) {
                #ifdef UART_DBG_PRINT
                chprintf(DBG_UART, "UART overrun error!\r\n");
                #endif
            } else {
                // Ensure that the message version is correct and bytes 4 and 5
                // are identical
                uint16_t rcvd_msg_ver;
		memcpy(&rcvd_msg_ver,&read_buffer[0],2);
                if (rcvd_msg_ver != MESSAGE_VERSION) {
                    #ifdef UART_DBG_PRINT
                    chprintf(DBG_UART, "UART message version incorrect!\r\n");
                    #endif
                } else if (read_buffer[4] != read_buffer[5]) {
                    #ifdef UART_DBG_PRINT
                    chprintf(DBG_UART, "UART command block incorrect!\r\n");
                    #endif
                } else {
                    // If correct, start parsing the command
                    switch (read_buffer[4]) {
                        case CMD_READY:
                            #ifdef UART_DBG_PRINT
                            chprintf(DBG_UART, "UART command ready query received\r\n");
                            #endif
                            // Check if the camera has failed
                            if (!cameraHealth) {
                                UART_Reply_Failed(write_cmd_buffer);
                            } else {
                                // Return the status of the camera data
                                if (cameraThreadDone == CAMERA_THREAD_BUSY) {
                                    UART_Reply_NReady(write_cmd_buffer);
                                } else if (cameraThreadDone == CAMERA_THREAD_DONE) {
                                    UART_Reply_Ready(write_cmd_buffer);
                                }
                            }
                            break;
                        case CMD_TRANS:
                            #ifdef UART_DBG_PRINT
                            chprintf(DBG_UART, "UART command transmit received\r\n");
                            #endif
                            // Check if the camera has failed
                            if (!cameraHealth) {
                                UART_Reply_Failed(write_cmd_buffer);
                            } else {
                                uint16_t numBytes;
                                numBytes = readLineFromSPI( line, &lineData[0] );
				//readLineFromSPI returns 0 if internal chksum
				//fails
				//TODO:  Check to see what desired operation is
				//when this happens
                                if(!numBytes){
				    UART_Reply_Failed(write_cmd_buffer);
				} else {
                                   UART_Reply_Data(line, numBytes, &lineData[0],write_cmd_buffer); 
				}
                                line++;
                            }
                            break;
                        default:
                            // ???
                            // PROFIT!
                            break;
                    }
                }
            }
        }
    }
    return 0;
}

