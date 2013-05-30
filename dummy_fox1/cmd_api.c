#include "cmd_api.h"

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
    char msg[2];
#if DUMMY_IHU
    if (msgtype == TT) {
        msg[0] = 'T';
        msg[1] = 'T';
    }
    else if (msgtype == RR) {
        msg[0] = 'R';
        msg[1] = 'R';
    } 
#elif DUMMY_EXP4
    if (msgtype == NN) {
        msg[0] = 'N';
        msg[1] = 'N';
    }
    else if (msgtype == YY) {
        msg[0] = 'Y';
        msg[1] = 'Y';
    }
    else if (msgtype == FF) {
        msg[0] = 'F';
        msg[1] = 'F';
    }
#endif
    else {
        printf("ERROR: msgtype %d not found\n",msgtype);
        exit(1);
    }

    if ((ret = write(ser,msg,sizeof msg)) == -1)
        printf("write failed with %d strerror(%s)\n",ret,strerror(-ret));
    else
        printf("sent %c%c (%d bytes)\n",msg[0],msg[1],ret);
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
    char rcvd[2];
    int ret = 0;
    int err = 0;

    if ((ret = read(ser,rcvd,sizeof rcvd)) == -1) {
        err = errno;
        printf("ERROR: read failed with %d strerror(%s)\n",err,strerror(err));
        return -1;
    }

    printf("(%c%c) received\n",rcvd[0],rcvd[1]);

#if DUMMY_IHU
    if ((rcvd[0] == 'N') && (rcvd[1] == 'N'))
        return NN;
    else if ((rcvd[0] == 'Y') && (rcvd[1] == 'Y'))
        return YY;
    else if ((rcvd[0] == 'F') && (rcvd[1] == 'F'))
        return FF;
#elif DUMMY_EXP4
    if ((rcvd[0] == 'R') && (rcvd[1] == 'R')) {
        return RR;
    }
    else if ((rcvd[0] == 'T') && (rcvd[1] == 'T')) {
        return TT;
    }
#endif
    else {
        printf("Reply (%c%c) not recognized\n",rcvd[0],rcvd[1]);
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

    printf (" --> get_IMG(int) is still unimplemented\n");
    /*
#for loop here
    msg = []
    for i in range(0,5):
        data = ser.read(1)
        msg.append(struct.unpack("B", data)[0])
        print 'msg[' + str(i) + '] = ' + str(msg[i])

    descriptor = msg[1];#struct.unpack("P", data)
    print 'd: ' + str(descriptor)

    data = ser.read(2)
    descriptor = struct.unpack("<H", data)[0]
    data1 = ser.read(1) #descriptor&0x3FF)
    data2 = ser.read(2)
    print 'd: ' + str(descriptor)

    linenum = descriptor >> 10
    payload = struct.unpack("<B", data1)[0]
    chksum = struct.unpack("<H", data2)[0]

    print 'p: ' + str(payload)
    print 'l: ' + str(linenum)
    print 'pl: ' + str(descriptor&0x3FF)
    print 'c: ' + str(chksum)
    */
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

    printf (" --> send_IMG(int) is still unimplemented\n");
    /*
#for loop here
    msg = []
    for i in range(0,5):
        data = ser.read(1)
        msg.append(struct.unpack("B", data)[0])
        print 'msg[' + str(i) + '] = ' + str(msg[i])

    descriptor = msg[1];#struct.unpack("P", data)
    print 'd: ' + str(descriptor)

    data = ser.read(2)
    descriptor = struct.unpack("<H", data)[0]
    data1 = ser.read(1) #descriptor&0x3FF)
    data2 = ser.read(2)
    print 'd: ' + str(descriptor)

    linenum = descriptor >> 10
    payload = struct.unpack("<B", data1)[0]
    chksum = struct.unpack("<H", data2)[0]

    print 'p: ' + str(payload)
    print 'l: ' + str(linenum)
    print 'pl: ' + str(descriptor&0x3FF)
    print 'c: ' + str(chksum)
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
    options.c_cflag = B38400 | CRTSCTS | CS8 | CLOCAL | CREAD;
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


