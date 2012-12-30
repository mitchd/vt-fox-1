#include "uart_iface.h"
//Commands to expect from the IHU
static const uint8_t CMD_TAKE_PIC = (uint8_t)'T';
static const uint8_t CMD_TELEMETRY  = (uint8_t)'Y';
static const uint8_t CMD_ABORT = (uint8_t)'A';

//Responses to send to the IHU
static const uint8_t RESP_OK = (uint8_t)'K';
static const uint8_t RESP_RX_ERR = (uint8_t)'E';
//
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
  //Track the size of our reads and writes to the serial port
  size_t readSize, writeSize;
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
      readSize = sdRead( &SD3, read_buffer, COMMAND_SIZE );
      chEvtClearFlags( IO_INPUT_AVAILABLE );
      //Check for RX errors
      if( events & SD_OVERRUN_ERROR ){
        //We had a buffer overrun in grabbing this data
        //Toggle the RED LED
        chprintf((BaseChannel *)&SD3, "%d %d\r\n", readSize, COMMAND_SIZE);
        palTogglePad(GPIOD, GPIOD_LED5);
        myUART_state = UART_COMM_ERROR;
	writeSize = sdAsynchronousWrite( &SD3, &RESP_RX_ERR, COMMAND_SIZE );
        chEvtClearFlags( SD_OVERRUN_ERROR );
      }else{
	#ifdef LOCAL_ECHO
        //We'll just echo for now and see what happens
        writeSize = sdAsynchronousWrite( &SD3, read_buffer, COMMAND_SIZE );
	sdAsynchronousWrite( &SD3, (uint8_t*)"\r\n", 2 );
	#endif
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
#endif
  return 0;
}
