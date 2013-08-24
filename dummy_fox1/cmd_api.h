#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>


//Messages
// IHU
#define RR 0
#define TT 1
// EXP4
#define NN 2
#define YY 3
#define FF 4

#define IHU_VER     100 //version 1.00
#define EXP4_VER    100 //version 1.00
#define MSG_VER     101 //version 1.01

//structs used in spi_flash.h
typedef struct {
    uint8_t line_num;
    uint32_t start_addr;
    uint32_t end_addr;
    uint8_t chksum;
} line_data;

typedef struct {
    line_data line[60];
} system_data;


//prototypes
void send_CMD(int ser, int msgtype);
int get_REPLY(int ser);
void get_IMG(int ser);
void send_IMG(int ser);
void print_HELP();
int serial_SETUP(char * serial_dev);
