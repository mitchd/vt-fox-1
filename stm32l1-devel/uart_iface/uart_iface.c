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

// #define INPUT_BUFFER_SIZE 128
// static uint8_t input_buffer[INPUT_BUFFER_SIZE]; //Input buffer

/*
static void showMenu(void){
    uint8_t i = 0;
    for( ; i < INPUT_BUFFER_SIZE; i++ )
        input_buffer[i] = 0x00;
    chprintf((BaseChannel*)&SD3, "\r\nDevelopment Menu\r\n");
    chprintf((BaseChannel*)&SD3, "(1) Write Byte to SPI Flash\r\n");
    chprintf((BaseChannel*)&SD3, "(2) Read Byte from SPI Flash\r\n");
    chprintf((BaseChannel*)&SD3, "(3) Write Bytes to SPI Flash\r\n");
    chprintf((BaseChannel*)&SD3, "(4) Read Bytes from SPI Flash\r\n");
    return;
}
static void showWriteBytePrompt(void){
    chprintf((BaseChannel*)&SD3, "\r\n<Hex Address> <Hex Byte>: 00100F Z\r\n");
    chprintf((BaseChannel*)&SD3, " :>");
}
static void showReadBytePrompt(void){
    chprintf((BaseChannel*)&SD3, "\r\n<Hex Address>: 00100F\r\n");
    chprintf((BaseChannel*)&SD3, " :>");
}
static void showWriteBytesPrompt(void){
    chprintf((BaseChannel*)&SD3, "\r\nStartng at 0x000000, enter number of bytes (max %d):\r\n",INPUT_BUFFER_SIZE);
    chprintf((BaseChannel*)&SD3, " :>");
}
static void showReadBytesPrompt(void){
    chprintf((BaseChannel*)&SD3, "\r\nReading from 0x000000, enter number of bytes:\r\n");
    chprintf((BaseChannel*)&SD3, " :>");
}
*/

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
            // Blocks until we have a set number of bytes
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
                            
                            break;
                        case CMD_TRANS:
                            #ifdef UART_DBG_PRINT
                            chprintf((BaseChannel*)&SD1, "UART command transmit received\r\n");
                            #endif
                            // Return camera data

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

/*
msg_t UART_Thread(void* arg){
    (void)arg;

    //Register this as an event listener for events from SD3
    EventListener serial_events;
    //Events received from the serial device
    const eventmask_t myUART_events = CHN_INPUT_AVAILABLE | 
                                        SD_OVERRUN_ERROR | 
                                        CHN_OUTPUT_EMPTY;
    chEvtRegisterMask( &(SD3.event), &serial_events, myUART_events );

    //Make our read buffer the same size as the buffer used with the serial device 
    uint8_t read_buffer[COMMAND_SIZE];
    
    //Keep track of our current communication state with myUART_state
    //This should be transformed to a global, signal, something....
    uint8_t myUART_state = UART_COMM_IDLE;

    //recorded events
    eventmask_t events;
#ifdef RELEASE_VERSION
    while (TRUE) {
        //Wait for a serial event:
        myUART_state = UART_COMM_IDLE;
        events = chEvtWaitOne( myUART_events );
        //Now we have an event
        if( events & CHN_INPUT_AVAILABLE ){
            //We have data available on the input
            myUART_state = UART_COMM_READ_COMMAND;
            sdRead( &SD3, read_buffer, COMMAND_SIZE );
            chEvtClearFlags( CHN_INPUT_AVAILABLE );
            //Check for RX errors
            if( events & SD_OVERRUN_ERROR ){
                //We had a buffer overrun in grabbing this data
                //Toggle the RED LED
                myUART_state = UART_COMM_ERROR;
                sdAsynchronousWrite( &SD3, &RESP_RX_ERR, COMMAND_SIZE );
                chEvtClearFlags( SD_OVERRUN_ERROR );
            }else{
                 //Parse the commands... if COMMAND_LENGTH changes, this needs to change
                 //I don't think there's a good way to make it generic
                if( read_buffer[0] == CMD_ABORT ){
                    //TODO: Send abort signal!
                    sdAsynchronousWrite( &SD3, &RESP_OK, COMMAND_SIZE );
                    sdAsynchronousWrite( &SD3, &CMD_ABORT, COMMAND_SIZE );
                }else if( read_buffer[0] == CMD_TELEMETRY ){
                    //TODO: Send telemetry signal
                    sdAsynchronousWrite( &SD3, &RESP_OK, COMMAND_SIZE );
                    sdAsynchronousWrite( &SD3, &CMD_TELEMETRY, COMMAND_SIZE );
                }else if( read_buffer[0] == CMD_TAKE_PIC ){
                    //TODO: Send take pic signal
                    sdAsynchronousWrite( &SD3, &RESP_OK, COMMAND_SIZE );
                    sdAsynchronousWrite( &SD3, &CMD_TAKE_PIC, COMMAND_SIZE );
                }else{
                    //TODO: Do we need to do anything with an error condition??
                    sdAsynchronousWrite( &SD3, &RESP_RX_ERR, COMMAND_SIZE );
                }
            }
        }
        if( events & CHN_OUTPUT_EMPTY )
            chEvtClearFlags( CHN_OUTPUT_EMPTY );
    }
#else
    uint8_t input_pos = 0; //Next byte input position
    myUART_state = UART_COMM_IDLE;
    while (TRUE) {
        //Wait for a serial event:
        events = chEvtWaitOne( myUART_events );
        //Now we have an event
        if( events & CHN_INPUT_AVAILABLE ){
            //We have data available on the input
            sdRead( &SD3, read_buffer, COMMAND_SIZE );
            chEvtClearFlags( CHN_INPUT_AVAILABLE );
            if( read_buffer[0] == 0x8 ){ //Handle some backspaceage
                input_pos--;
                if( input_pos == (uint8_t)(-1) )
                    input_pos = 0;
                    else{
                    #ifdef LOCAL_ECHO
                    sdAsynchronousWrite( &SD3, read_buffer, COMMAND_SIZE );
                    #endif
                }
            }else if( events & SD_OVERRUN_ERROR ){
                //We had a buffer overrun in grabbing this data
                //Toggle the RED LED
                palTogglePad(GPIOD, GPIOD_LED5);
                chEvtClearFlags( SD_OVERRUN_ERROR );
            }else{
    #ifdef LOCAL_ECHO
                sdAsynchronousWrite( &SD3, read_buffer, COMMAND_SIZE );
    #endif
                 //Parse the commands... 
                 //We're not going to do very good error checking here, because this
                 //isn't the primary function of this project.
                if( myUART_state == UART_COMM_IDLE ){
                    input_buffer[input_pos] = read_buffer[0];
                    if( input_buffer[input_pos] == (uint8_t)'\r' ){
                        char option = (char)input_buffer[0];
                        //chprintf((BaseChannel*)&SD3,"\r\n%c Selected\r\n",option);
                        //Reset the pos pointer
                        input_pos = 0;
                        if( option == '1' ){
                            myUART_state = UART_WRITE_SPI_BYTE;
                            showWriteBytePrompt();
                        }else if( option == '2' ){
                            myUART_state = UART_READ_SPI_BYTE;
                            showReadBytePrompt();
                        }else if( option == '3' ){
                            myUART_state = UART_RX_RGB;
                            showWriteBytesPrompt();
                        }else if( option == '4' ){
                            myUART_state = UART_TX_RGB;
                            showReadBytesPrompt();
                        }else
                            showMenu();
                     }else{
                         if(input_pos == INPUT_BUFFER_SIZE-1){
                             input_pos = 0;
                             //buffer flush?
                         }else
                             input_pos++;
                     }
                }else if( myUART_state == UART_WRITE_SPI_BYTE ){
                    input_buffer[input_pos] = read_buffer[0];
                    if( input_buffer[input_pos] == (uint8_t)'\r' ){
                        chprintf((BaseChannel*)&SD3,"\r\nDestination: ");
                        sdWrite(&SD3,input_buffer,6);
                        chprintf((BaseChannel*)&SD3,"\r\nData: ");
                        sdWrite(&SD3,input_buffer+7,1);
                        //Convert the ASCII destination address to a 32bit value
                        uint32_t dest;
                        uint32_t dest32 = 0;
                        uint8_t i;
                        for( i = 0; i < 6; i++ ){
                            if( (char)input_buffer[i] >= 'a' )
                                dest = input_buffer[i] - (uint8_t)'a' + 0x10;
                            else if( (char)input_buffer[i] >= 'A' )
                                dest = input_buffer[i] - (uint8_t)'A' + 0x10;
                            else
                                dest = input_buffer[i] - (uint8_t)'0';
                            dest32 |= dest<<(4*(5-i));
                        }
                        //Write to SPI
                        flashWriteByte( dest32, *(input_buffer+7) );
                        myUART_state = UART_COMM_IDLE;
                        showMenu();
                        input_pos = 0;
                    }else{
                        if(input_pos == INPUT_BUFFER_SIZE-1){
                            input_pos = 0;
                            //buffer flush?
                        }else
                            input_pos++;
                    }
                }else if( myUART_state == UART_READ_SPI_BYTE ){
                    input_buffer[input_pos] = read_buffer[0];
                    if( input_buffer[input_pos] == (uint8_t)'\r' ){
                        chprintf((BaseChannel*)&SD3,"\r\nTarget: ");
                        sdWrite(&SD3,input_buffer,6);
                        //Convert the ASCII destination address to a 32bit value
                        uint32_t dest;
                        uint32_t dest32 = 0x0;
                        uint8_t i;
                        for( i = 0; i < 6; i++ ){
                            if( (char)input_buffer[i] >= 'a' )
                                dest = input_buffer[i] - (uint8_t)'a' + 0x10;
                            else if( (char)input_buffer[i] >= 'A' )
                                dest = input_buffer[i] - (uint8_t)'A' + 0x10;
                            else
                                dest = input_buffer[i] - (uint8_t)'0';
                            dest32 |= dest<<(4*(5-i));
                        }
                        //Write to SPI
                        uint8_t read = flashReadByte( dest32 );
                        chprintf((BaseChannel*)&SD3,"\r\nData: ");
                        sdWrite(&SD3,&read,1);
                        myUART_state = UART_COMM_IDLE;
                        showMenu();
                        input_pos = 0;
                    }else{
                        if(input_pos == INPUT_BUFFER_SIZE-1){
                            input_pos = 0;
                            //buffer flush?
                        }else
                            input_pos++;
                    }
                }else if( myUART_state == UART_RX_RGB ){
                    input_buffer[input_pos] = read_buffer[0];
                    if( input_buffer[input_pos] == (uint8_t)'\r' ){
                        uint32_t bytes = 0;
                        uint32_t mult = 1;
                        uint8_t i;
                        for( i = 0; i < input_pos-1; i++ ){
                            mult *= 10;
                        }
                        for( i = 0; i < input_pos; i++ ){
                            bytes += (input_buffer[i] - (uint8_t)'0') * mult;
                            mult /= 10;
                        }
                        chprintf((BaseChannel*)&SD3,"\r\nNum Bytes: %d", bytes);
                        chprintf((BaseChannel*)&SD3,"\r\nErasing Flash\r\n");
                        chprintf((BaseChannel*)&SD3,"\r\nErasing Done, Transfer %d bytes\r\n", bytes);
                        for( i = 0; i < bytes; i++ ){
                            sdRead( &SD3, (input_buffer+i), 1 );
                            #ifdef LOCAL_ECHO
                            sdAsynchronousWrite( &SD3, (input_buffer+i), COMMAND_SIZE );
                            #endif
                        }
                        flashWriteBytes( 0x00, input_buffer, bytes );
                        myUART_state = UART_COMM_IDLE;
                        showMenu();
                    }else{
                        if(input_pos == INPUT_BUFFER_SIZE-1){
                            input_pos = 0;
                            //buffer flush?
                        }else
                            input_pos++;
                    }
                }else if( myUART_state == UART_TX_RGB ){
                    input_buffer[input_pos] = read_buffer[0];
                    if( input_buffer[input_pos] == (uint8_t)'\r' ){
                        uint32_t bytes = 0;
                        uint32_t mult = 1;
                        uint8_t i;
                        for( i = 0; i < input_pos-1; i++ ){
                            mult *= 10;
                        }
                        for( i = 0; i < input_pos; i++ ){
                            bytes += (input_buffer[i] - (uint8_t)'0') * mult;
                            mult /= 10;
                        }
                        for( i = 0; i < INPUT_BUFFER_SIZE; i++ )
                            input_buffer[i] = 0x00;
                        chprintf((BaseChannel*)&SD3,"\r\nNum Bytes: %d\r\n", bytes);
                        flashReadBytes( 0x000000, input_buffer, bytes );
                        sdWrite( &SD3, input_buffer, bytes);
                        myUART_state = UART_COMM_IDLE;
                        showMenu();
                    }else{
                        if(input_pos == INPUT_BUFFER_SIZE-1){
                            input_pos = 0;
                            //buffer flush?
                        }else
                            input_pos++;
                    }
                }
            }
        }
        if( events & CHN_OUTPUT_EMPTY )
            chEvtClearFlags( CHN_OUTPUT_EMPTY );
    }
#endif
    return 0;
}
*/