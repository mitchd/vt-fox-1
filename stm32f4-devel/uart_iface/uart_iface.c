#include "uart_iface.h"
//Commands to expect from the IHU
static const uint8_t CMD_TAKE_PIC = (uint8_t)'T';
static const uint8_t CMD_TELEMETRY  = (uint8_t)'Y';
static const uint8_t CMD_ABORT = (uint8_t)'A';

//Responses to send to the IHU
static const uint8_t RESP_OK = (uint8_t)'K';
static const uint8_t RESP_RX_ERR = (uint8_t)'E';

#ifndef RELEASE_VERSION
#define INPUT_BUFFER_SIZE 128
static uint8_t input_buffer[INPUT_BUFFER_SIZE]; //Input buffer
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
#endif
/*
 * UART_Thread
 *
 * This is the thread that handle all the UART communications for the board
 *
 * It's primary tasks are:
 *
 * 	1) Receive and parse commands from the IHU
 * 	2) Signal the main thread to initiate a camera capture
 * 	3) Send requested data to the IHU
 * 		A) Telemetry (if implemented)
 * 		B) jpeg data
 *
 * As these additional feature are implemented *arg will begin to develop and
 * documentation about the argument parameters will be provided.
 *
 *
 * Key internal variables in this function:
 *
 * serial_events:
 * 	This is our event listener.  It is registered to the serial driver.  As
 * 	our error-handling code becomes more sophisticated, we can register all
 * 	possible serial events.
 *
 * myUART_events:
 * 	This is the event mask that gets registered to the serial driver.  Event
 * 	names are fairly self-explanatory
 *
 *COMMAND_SIZE (defined in uart_iface.h):
 *	We expect commands from the IHU to be a certain length (1-4 bytes,
 *	depending on the error correction we want in the actual commands or
 *	whatev).
 *
 *readSize, writeSize:
 *	Track the number of bytes read or written to the serial device for error
 *	handling.
 *
 *read_buffer[COMMAND_SIZE]:
 *	We only expect to receive COMMAND_SIZE chunks of data from the IHU.
 *	With the exception of some debug/development-specific code (e.g. to
 *	transfer data from camera to computer, or rgb data from computer to flash
 *	We will also only give COMMAND_SIZE chunks to the uC
 *
 */
msg_t UART_Thread(void* arg){
  (void)arg;

  //Register this as an event listener for events from SD3
  EventListener serial_events;
  //Events received from the serial device
  const eventmask_t myUART_events = IO_INPUT_AVAILABLE | 
                                    SD_OVERRUN_ERROR | 
                                    IO_OUTPUT_EMPTY;
  chEvtRegisterMask( &(SD3.event), &serial_events, myUART_events );

  //Make our read buffer the same size as the buffer used with the serial device 
  uint8_t read_buffer[COMMAND_SIZE];
  //
  //Keep track of our current communication state with myUART_state
  //This should be transformed to a global, signal, something....
  uint8_t myUART_state = UART_COMM_IDLE;
  //recorded events
  eventmask_t events;
/*
 *Release code
 *
 *Enable this by #define RELEASE_VERSION somewhere
 *
 */
#ifdef RELEASE_VERSION
  while (TRUE) {
    //Wait for a serial event:
    myUART_state = UART_COMM_IDLE;
    events = chEvtWaitOne( myUART_events );
    //Now we have an event
    if( events & IO_INPUT_AVAILABLE ){
      //We have data available on the input
      myUART_state = UART_COMM_READ_COMMAND;
      sdRead( &SD3, read_buffer, COMMAND_SIZE );
      chEvtClearFlags( IO_INPUT_AVAILABLE );
      //Check for RX errors
      if( events & SD_OVERRUN_ERROR ){
        //We had a buffer overrun in grabbing this data
        //Toggle the RED LED
        myUART_state = UART_COMM_ERROR;
	sdAsynchronousWrite( &SD3, &RESP_RX_ERR, COMMAND_SIZE );
        chEvtClearFlags( SD_OVERRUN_ERROR );
      }else{
	/*
         *Parse the commands... if COMMAND_LENGTH changes, this needs to change
         *I don't think there's a good way to make it generic
 	 */
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
    if( events & IO_OUTPUT_EMPTY )
      chEvtClearFlags( IO_OUTPUT_EMPTY );
  }
#else
/*
 *Development code
 *
 *Enable this by ensuring RELEASE_VERSION is undefined
 *
 */
  uint8_t input_pos = 0; //Next byte input position
  myUART_state = UART_COMM_IDLE;
  while (TRUE) {
    //Wait for a serial event:
    events = chEvtWaitOne( myUART_events );
    //Now we have an event
    if( events & IO_INPUT_AVAILABLE ){
      //We have data available on the input
      sdRead( &SD3, read_buffer, COMMAND_SIZE );
      chEvtClearFlags( IO_INPUT_AVAILABLE );
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
	/*
         *Parse the commands... 
         *We're not going to do very good error checking here, because this
         *isn't the primary function of this project.
 	 */
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
              flashErase( 0x00000000, FLASH_4KB_ERASE, TRUE );
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
         
         }
       }
    }
    if( events & IO_OUTPUT_EMPTY )
      chEvtClearFlags( IO_OUTPUT_EMPTY );
  }
#endif
  return 0;
}

