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
 *  2) Signal the main thread to initiate a camera capture
 *  3) Send requested data to the IHU
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
 *  We expect commands from the IHU to be a certain length (1-4 bytes,
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
    buffer[0] = MESSAGE_VERSION;
    buffer[2] = SOFTWARE_BUILD;
    buffer[4] = RESP_FAILED;
    buffer[5] = RESP_FAILED;
    sdAsynchronousWrite(&SD3, buffer, MESSAGE_CMD_REPLY_SIZE);
}

void UART_Reply_NReady(uint8_t *buffer) {
    buffer[0] = MESSAGE_VERSION;
    buffer[2] = SOFTWARE_BUILD;
    buffer[4] = RESP_NREADY;
    buffer[5] = RESP_NREADY;
    sdAsynchronousWrite(&SD3, buffer, MESSAGE_CMD_REPLY_SIZE);
}

void UART_Reply_Ready(uint8_t *buffer) {
    buffer[0] = MESSAGE_VERSION;
    buffer[2] = SOFTWARE_BUILD;
    buffer[4] = RESP_READY;
    buffer[5] = RESP_READY;
    sdAsynchronousWrite(&SD3, buffer, MESSAGE_CMD_REPLY_SIZE);
}

void UART_Reply_Data(uint8_t line_ID, uint16_t length, uint8_t *data, uint8_t *buffer) {
    buffer[0] = MESSAGE_VERSION;
    buffer[2] = SOFTWARE_BUILD;
    // Line ID is the 6 MSB and length is 10 LSB in the first two bytes
    buffer[4] = length & 0xFF;
    buffer[5] = (line_ID << 2) | ((length & 0x300) >> 8);
    // Copy the data to send to the buffer
    uint16_t i, chksum = 0;
    for (i = 0; i < length; i++) {
        buffer[6+i] = data[i];
        chksum += data[i];
    }
    // Write the checksum to the buffer
    buffer[6+length+0] = chksum;
    buffer[6+length+1] = chksum >> 8;

    sdAsynchronousWrite(&SD3, buffer, MESSAGE_DATA_REPLY_SIZE + length);
}

msg_t UART_Thread(void* arg) {
    (void) arg;

    // Register this as an event listener for events from SD3
    EventListener serial_events;
    // Events received from the serial device
    const eventmask_t myUART_events = CHN_INPUT_AVAILABLE | 
                                        SD_OVERRUN_ERROR | 
                                        CHN_OUTPUT_EMPTY;
    chEvtRegisterMask( &(SD3.event), &serial_events, myUART_events );

    // Make our read buffer the same size as the buffer used with the serial device 
    uint8_t read_buffer[MESSAGE_CMD_SIZE];
    uint8_t write_cmd_buffer[MESSAGE_CMD_REPLY_SIZE] = {0};
    uint8_t write_data_buffer[MESSAGE_DATA_SIZE];
    uint8_t i;

    // Recorded events
    eventmask_t events;

    while (TRUE) {
        // Wait for a serial event
        events = chEvtWaitOne(myUART_events);

        if (events & CHN_INPUT_AVAILABLE) {
            // Blocks until we have read a set number of bytes
            sdRead(&SD3, read_buffer, MESSAGE_CMD_SIZE);
            chEvtClearFlags(CHN_INPUT_AVAILABLE);

            #ifdef UART_DBG_PRINT
            chprintf((BaseChannel*)&SD1, "UART data received: \r\n");
            for (i = 0; i < MESSAGE_CMD_SIZE; i++) {
                chprintf((BaseChannel*)&SD1, "%02X ", read_buffer[i]);
            }
            chprintf((BaseChannel*)&SD1, "\r\n");
            #endif

            // First check for transmission errors
            if (events & SD_OVERRUN_ERROR) {
                #ifdef UART_DBG_PRINT
                chprintf((BaseChannel*)&SD1, "UART overrun error!\r\n");
                #endif
                chEvtClearFlags(SD_OVERRUN_ERROR);
            } else {
                // Ensure that the message version is correct and bytes 4 and 5 are identical
                if (read_buffer[0] != MESSAGE_VERSION || read_buffer[1] != 0x0) {
                    #ifdef UART_DBG_PRINT
                    chprintf((BaseChannel*)&SD1, "UART message version incorrect!\r\n");
                    #endif
                } else if (read_buffer[4] != read_buffer[5]) {
                    #ifdef UART_DBG_PRINT
                    chprintf((BaseChannel*)&SD1, "UART command block incorrect!\r\n");
                    #endif
                } else {
                    // If correct, start parsing the command
                    switch (read_buffer[4]) {
                        case CMD_READY:
                            #ifdef UART_DBG_PRINT
                            chprintf((BaseChannel*)&SD1, "UART command ready query received\r\n");
                            #endif
                            // Poll for and return camera status
                            if (!cameraHealth) {
                                UART_Reply_Failed(write_cmd_buffer);
                            } else {
                                if (cameraThreadDone == CAMERA_THREAD_BUSY) {
                                    UART_Reply_NReady(write_cmd_buffer);
                                } else if (cameraThreadDone == CAMERA_THREAD_DONE) {
                                    UART_Reply_Ready(write_cmd_buffer);
                                }
                            }
                            break;
                        case CMD_TRANS:
                            #ifdef UART_DBG_PRINT
                            chprintf((BaseChannel*)&SD1, "UART command transmit received\r\n");
                            #endif
                            // Return camera data
                            if (!cameraHealth) {
                                UART_Reply_Failed(write_cmd_buffer);
                            } else {

                            }
                            break;
                        default:
                            // ???
                            break;
                    }
                }
            }
        }
        // No idea what this event is for, but we clear it anyways
        if (events & CHN_OUTPUT_EMPTY)
            chEvtClearFlags(CHN_OUTPUT_EMPTY);
    }
    return 0;
}
