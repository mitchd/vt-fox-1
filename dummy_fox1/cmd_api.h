#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

//Messages
// IHU
#define RR 0
#define TT 1
// EXP4
#define NN 2
#define YY 3
#define FF 4


//prototypes
void send_CMD(int ser, int msgtype);
int get_REPLY(int ser);
void get_IMG(int ser);
void send_IMG(int ser);
void print_HELP();
int serial_SETUP(char * serial_dev);
