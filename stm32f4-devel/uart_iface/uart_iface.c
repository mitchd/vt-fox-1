#include "uart_iface.h"

//Thread function
msg_t UART_Thread(void* arg){
  (void)arg;

  //Register this as an event listener for events from SD3
  EventListener serial_events;
  //Events received from the serial device
  const eventmask_t myUART_events = IO_NO_ERROR | IO_INPUT_AVAILABLE | SD_OVERRUN_ERROR | IO_OUTPUT_EMPTY;
  chEvtRegisterMask( &(SD3.event), &serial_events, myUART_events );

  //Make our read buffer the same size as the buffer used with the serial device 
  uint8_t read_buffer[SERIAL_BUFFERS_SIZE];
  //Keep track of our current communication state with myUART_state
  uint8_t myUART_state = UART_COMM_IDLE;
  //Track the size of our reads and writes to the serial port
  size_t readSize, writeSize;
  //recorded events
  eventmask_t events;
  while (TRUE) {
    //Wait for a serial event:
    events = chEvtWaitOne( myUART_events );
    //Now we have an event
    if( events & IO_NO_ERROR )
      chEvtClearFlags( IO_NO_ERROR );
    if( events & IO_INPUT_AVAILABLE ){
      //We have data available on the input
      readSize = sdAsynchronousRead( &SD3, read_buffer, 1 );
      chEvtClearFlags( IO_INPUT_AVAILABLE );
      if( events & SD_OVERRUN_ERROR ){
        //We had a buffer overrun in grabbing this data
        //Toggle the RED LED
        palTogglePad(GPIOD, GPIOD_LED5);
        chEvtClearFlags( SD_OVERRUN_ERROR );
      }
      if( readSize > 0 ){
        //We'll just echo for now and see what happens
        writeSize = sdAsynchronousWrite( &SD3, read_buffer, 1 );
      }
    }
    if( events & IO_OUTPUT_EMPTY )
      chEvtClearFlags( IO_OUTPUT_EMPTY );
  }
  return 0;
}
