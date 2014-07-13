/*******************************************************************************
# CMD_API:                                                                    #
# --------------------------------------------------------------------------- #
# This API contains abstracted functions to send and receive messages from    #
# IHU. It is designed to represent the IHU or the EXP4 board determined       #
# at compile time.                                                            #
#                                                                             #
#   Author: Kevin Burns                                                       #
*******************************************************************************/


#include "cmd_api.h"
#include "time.h"
#include "fcntl.h"

/*******************************************************************************
# SEND_CMD (IHU)
#   This sends correctly formatted message to the experiment 4 board. As 
#   specified in the documentation, this can be one of two types: RR or TT
#   
#   Message Types:
#       (RR) Camera Ready?
#       (TT) Transmit Data Block
#
# SEND_CMD (EXP4)
#   This sends correctly formatted message to the experiment 4 board. As 
#   specified in the documentation, this can be one of three types: NN, YY, or 
#   FF.
#   
#   Message Types:
#       (NN) Camera Not Ready
#       (YY) Camera is Ready
#       (FF) Camera has failed
*******************************************************************************/
void send_CMD(int ser, int msgtype) {
    int ret = 0;
    char msg[6]; //4 bytes for header

    //Add command to message
#if DUMMY_IHU
    if (msgtype == TT) {
        msg[4] = 'T';
        msg[5] = 'T';
    }
    else if (msgtype == RR) {
        msg[4] = 'R';
        msg[5] = 'R';
    } 
#elif DUMMY_EXP4
    if (msgtype == NN) {
        msg[4] = 'N';
        msg[5] = 'N';
    }
    else if (msgtype == YY) {
        msg[4] = 'Y';
        msg[5] = 'Y';
    }
    else if (msgtype == FF) {
        msg[4] = 'F';
        msg[5] = 'F';
    }
#endif
    else {
        printf("ERROR: msgtype %d not found\n",msgtype);
        exit(1);
    }

    //Add header
    msg[1] = (MSG_VER>>8)&0xFF;
    msg[0] = MSG_VER&0xFF;     
#if DUMMY_IHU
    msg[3] = IHU_VER>>8;
    msg[2] = IHU_VER&0xFF;
#elif DUMMY_EXP4
    msg[3] = EXP4_VER>>8;
    msg[2] = EXP4_VER&0xFF;
#endif

    //Send Message
    if ((ret = write(ser,msg,sizeof msg)) == -1)
        printf("write failed with %d strerror(%s)\n",ret,strerror(-ret));
    else
        printf("sent %c%c (%d bytes)\n",msg[4],msg[5],ret);
}

/*******************************************************************************
# GET_REPLY (IHU)
#   This gets the reply sent from the exp 4 board. The reply types are laid
#   out in more detail in the documentation mentioned at the top of this 
#   script. This function will BLOCK until a reply is received.
#   
#   Message Types:
#       (YY) Camera is ready
#       (NN) Camera is not ready
#       (FF) Camera has failed
#
# GET_REPLY (EXP4)
#   This gets the reply sent from the exp 4 board. The reply types are laid
#   out in more detail in the documentation mentioned at the top of this 
#   script. This function will BLOCK until a reply is received.
#   
#   Message Types:
#       (RR) Is Camera Ready?
#       (TT) Transmit Data Block
*******************************************************************************/
int get_REPLY(int ser) {
    char rcvd[6];
    uint16_t msg_ver = 0;
#if DUMMY_IHU 
    int ihu_ver = 0; 
#elif DUMMY_EXP4
    int exp4_ver = 0;
#endif
    int ret = 0;
    int err = 0;

    if ((ret = read(ser,rcvd,6)) == -1) {
        err = errno;
        printf("ERROR: read failed with %d strerror(%s)\n",err,strerror(err));
        exit(1);
        return -1;
    }

    printf("(%c%c) received\n",rcvd[4],rcvd[5]);

    //Verify versions
    memcpy(&msg_ver,rcvd,2);    //sizeof(uint16_t));

    if (MSG_VER != msg_ver) {
        printf("WARNING: msg version mismatch %d vs %d\n",MSG_VER,msg_ver);
    }
#if DUMMY_IHU
    if ((rcvd[4] == 'N') && (rcvd[5] == 'N'))
        return NN;
    else if ((rcvd[4] == 'Y') && (rcvd[5] == 'Y'))
        return YY;
    else if ((rcvd[4] == 'F') && (rcvd[5] == 'F'))
        return FF;
#elif DUMMY_EXP4
    if ((rcvd[4] == 'R') && (rcvd[5] == 'R')) {
        return RR;
    }
    else if ((rcvd[4] == 'T') && (rcvd[5] == 'T')) {
        return TT;
    }
#endif
    else {
        printf("Reply (%c%c) not recognized\n",rcvd[4],rcvd[5]);
        return -1;
    }
}


/******************************************************************************* 
# GET_IMG
#   This gets the data blocks the exp 4 board produces after the TT command
#   is received. There should be 60 messages (perfect world with no drops). 
#   This function also handles checksum checking.
#   
#   Message Breakdown:
#       DESCRIPTOR  : 2B (6b line number)(10b length in bytes)
#       PAYLOAD     : Variable but smaller than 1023B   
#       CHKSUM      : 2B accumulator sum of bytes in HEADER and PAYLOAD 
#   
*******************************************************************************/
void get_IMG(int ser) {

    int err = 0;
    int outputfile;
    const char * name = "output.dat\0";
    printf("Camera Ready to transmit, opening %s for writing\n", name);
    outputfile = open(name, O_WRONLY | O_CREAT );
    if(!outputfile){
      printf("File Open Failed\n");
      return;
    }
    char header[6];
    int bytes;
    for(int i = 0; i<60; i++){
       send_CMD(ser,TT);
       if((bytes = read(ser,header,6)) == -1) {
        err = errno; 
        printf("ERROR: read failed with %d strerror(%s)\n",err,strerror(err));
        close(ser);
        close(outputfile);
        exit(1);
       }
       //if(bytes != 6){
       // printf("ERROR: Only %d bytes read\n",bytes);
       // close(ser);
       // close(outputfile);
       // exit(1);
       //}
       int linenum = (header[5]>>2)&0x3F;
       int payloadsize = (((int)(header[5]&0x3))<<2) | header[4];
       printf("Line Number: %d\n", linenum);
       printf("Payload Size: %d\n", payloadsize);
       if(payloadsize < 0 || payloadsize > 1023){
         //something is terribly wrong
         printf("Badness in header\n");
         close(outputfile);
         close(ser);
         return;
       }
       char * data = (char*)malloc(payloadsize);
       //receive line
       if(read(ser,data,payloadsize)==-1){
         err = errno;
         printf("ERROR: read failed with %d strerror(%s)\n",err,strerror(err));
         close(outputfile);
         close(ser);
         exit(1);
       }
       if((payloadsize = write(outputfile,data,payloadsize))==-1){
         printf("Error writing to output file\n");
         err = errno;
         printf("ERROR: write failed with %d strerror(%s)\n",err,strerror(err));
         close(outputfile);
         close(ser);
         exit(1);
       }
       printf("Wrode %d bytes\n",payloadsize);
       int chksum;
       for (i = 0; i < payloadsize; i++) {
         chksum += data[i];
       }
       chksum = chksum&0xFFFF;
       read(ser,data,2);
       int rxchksum = data[0] + (((int)data[1])<<8);
       printf("Computed Checksum %d\n",chksum);
       printf("Recieved Checksum %d\n",rxchksum);
    }
}


/******************************************************************************* 
# SEND_IMG
#   This sends the data blocks the exp 4 board produces after the TT command
#   is received. There should be exactly 60 messages sent.
#   
#   Message Breakdown:
#       DESCRIPTOR  : 2B (6b line number)(10b length in bytes)
#       PAYLOAD     : Variable but smaller than 1023B   
#       CHKSUM      : 2B accumulator sum of bytes in HEADER and PAYLOAD 
#   
*******************************************************************************/
void send_IMG(int ser) {

    //printf (" --> send_IMG(int) is still unimplemented\n");

    //uint16_t desc = 0;
    //int i = 0;
    int j = 0;
    char msg[6];
    FILE * fp;
    char first[1],second[1];

    //This needs to be moved to after the file is read
    //msg = (char *)malloc(6);

    //Add header to message
    msg[1] = MSG_VER>>8;
    msg[0] = MSG_VER;     
#if DUMMY_IHU
    msg[3] = IHU_VER>>8;
    msg[2] = IHU_VER;
#elif DUMMY_EXP4
    msg[3] = EXP4_VER>>8;
    msg[2] = EXP4_VER;
#endif

    fp = fopen("imgs/640x480_test.jpg","w+");

    printf("DEBUG: top of while\n");
    while(fscanf(fp,"%c",first)) {
        if (atoi(first) == 0xFF) {
           printf("DEBUG: found 0xFF\n");
           fscanf(fp,"%c",second); 
           if (((atoi(second)>>4) == 0xD) && ((atoi(second) & 0xF) < 8)) {
                //found RST marker 
                j++;
                printf("found RST marker %d\n",j);
           }
        }
    }

    /* Note */
    /**************************************************************************
     * I added the structs to simulate the process EXP4 will take. These 
     * structs will be filled straight from the MRAM.
     *************************************************************************/
    /*
    system_data sd;
    line_data ld;

    //search for RST in jpg and fill line data
    for (i=0;i<(60);i++) {
        ld[i] = ;
    }

    for (i=1;i<(60+1);i++) {

        //Add descriptor
        desc = (i<<10) + ;  //add in payloads

        //pack desc into two chars
        msg[4] = (desc>>8);
        msg[5] = (desc&0xFF); 
    }
    */
}

void print_HELP() {
#if DUMMY_IHU
    printf( "usage: ./dummy_ihu <flags>\n");
#elif DUMMY_EXP4
    printf( "usage: ./dummy_exp4 <flags>\n");
#endif

    printf( "\n");
    printf( "flags\n");
    printf( "----------------------------------------\n");
    printf( "-h,   :   Prints this message\n");
    printf( "-s,   :   Specifies the serial device to be used\n");
    printf( "-m,   :   Sends just one message of specified type.\n");

#if DUMMY_IHU
    printf( "      :   Which currently is 1 (TT) and 0 (RR).\n");
#elif DUMMY_EXP4
    printf( "      :   Which currently is 2 (NN), 3 (YY), and 4 (FF).\n");
#endif
}

int serial_SETUP(char * ser_device) {

    int fd,ret;
    struct termios options;

    fd = open(ser_device, O_RDWR | O_NOCTTY ); //| O_SYNC);
    if(fd == -1) {
        printf("ERROR: Unable to open (%s)\n",ser_device);
    }
    else {
        //fcntl(fd, F_SETFL, 0);
        printf("Initializing the %s serial device...\n", ser_device);
    }

    if ((ret = tcgetattr(fd, &options)) != 0) 
        printf("ERROR: tcgetattr failed with %d strerror(%s)\n",ret,strerror(-ret));

    /*
    cfsetispeed(&options, B38400);
    cfsetospeed(&options, B38400);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~( ICANON | ECHO | ECHOE |ISIG );
    options.c_iflag &= ~(IXON | IXOFF | IXANY );
    options.c_oflag &= ~OPOST;
    */
    options.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
    options.c_iflag = 0;//IGNCR;
    options.c_oflag = 0;
    options.c_lflag = 0;//ICANON;
    //options.c_cc[VMIN]=1;
    //options.c_cc[VTIME]=0;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&options);


    //tcsetattr(fd, TCSANOW, &options);

/*
    printf("Test msg sent\n");
    if ((ret = write(fd,"TEST",4)) == -1)
        printf("write failed with %d strerror(%s)\n",ret,strerror(ret));
    else
        printf("write success\n");
*/

    return fd;
}


