/*******************************************************************************
# DUMMY IHU:                                                                  #
# --------------------------------------------------------------------------- #
#   This program simulates the IHU for the purpose of testing and debugging   #
#   the communiction protocols laid out in "AMSAT Fox-1A: IHU to Experiment 4 #
#   Interface Control Document".                                              #
#                                                                             #
#   Author: Kevin Burns                                                       #
*******************************************************************************/

#include "cmd_api.h"

//Defines
#define DEFAULT_DEV "/dev/ttyS0"
#define DEFAULT_BAUDRATE 38400

/*******************************************************************************
# MAIN                                                              
#   This contains argument parsing along with the main event loop
*******************************************************************************/
int main(int argc,char **argv) {
    char * serial_dev = DEFAULT_DEV;
    int ser = 0;
    int opt = 0;

    /* Input Argument Parsing */
    while ((opt = getopt(argc, argv, "h:sm:")) != -1) {

        switch(opt) {
            case 'h':
                print_HELP();
                exit(1);
                break;
            case 's':
                serial_dev = optarg;
                break;
            case 'm':
                ser = serial_SETUP(serial_dev); 
                send_CMD(ser,atoi(optarg));
                close(ser);
                exit(1);
                break;
            default:
                print_HELP();
                exit(1);
        }
    }

    printf("Selected %s as experiment 4 board\n", serial_dev);

    ser = serial_SETUP(serial_dev); 

    /*## Main Event Loop ##*/
#if DUMMY_IHU
    printf( "\nBEGINNING OF IHU COMMUNICATIONS\n");
#elif DUMMY_EXP4
    printf( "\nBEGINNING OF EXPIREMENT 4 COMMUNICATIONS\n");
#endif
    printf( "------------------------------------------------------------\n\n");

    int state = 0;
#if DUMMY_IHU
    printf( " -> This is where we would power on the board and wait 100ms\n"); 
    //Initiate communication
    send_CMD(ser,RR);
    //close(ser);
    //exit(1);
    //for debug ^
    while (1) {
        state = get_REPLY(ser);
        switch (state) {
            case YY:
                send_CMD(ser,TT);
                get_IMG(ser);
                break;
            case FF:
                printf( " -> This is where we would power cycle the board\n"); 
                send_CMD(ser,RR);
                break;
            case NN:
                sleep(1);
                send_CMD(ser,RR);
                break;
        }
    }
#elif DUMMY_EXP4
    while (1) {
        //IHU sends first command
        state = get_REPLY(ser);
        switch (state) {
            case TT:
                printf( " -> This is where we would start the image transfer\n"); 
                send_IMG(ser);
                break;
            case RR:
                send_CMD(ser,YY);
                break;
        }
    }
#endif
    close(ser);
}
