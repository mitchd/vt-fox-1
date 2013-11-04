#!/usr/bin/python2.7

###############################################################################
# DUMMY IHU:                                                                  #
# --------------------------------------------------------------------------- #
#   This script simulates the IHU for the purpose of testing and debugging    #
#   the communiction protocols laid out in "AMSAT Fox-1A: IHU to Experiment 4 #
#   Interface Control Document".                                              #
#                                                                             #
#   Author: Kevin Burns                                                       #
###############################################################################

import serial
import sys
import getopt
import string
import time
import struct

# Defines
DEFAULT_DEV = "/dev/ttyACM0"
DEFAULT_BAUDRATE = 38400
# Messages
# IHU
RR = 0
TT = 1
# EXP4
NN = 2
YY = 3
FF = 4


# State
    
###############################################################################
# MAIN                                                              
#   This contains argument parsing along with the main event loop
###############################################################################
def main(argv):   
    serial_dev = DEFAULT_DEV

    ## Input Argument Parsing ##
    try:
        opts, args = getopt.getopt(argv,"hs:m:",["ser=","msg="])
    except getopt.GetoptError:
        print 'usage: dummy_ihu.py -s <path to serial device>'
        sys.exit(2)
        
    for opt, arg in opts:
        if opt in ('-h', "--help"):
            print 'usage: ./dummy_ihu.py <flags>'
            print ''
            print 'flags '
            print '----------------------------------------'
            print '-h,--help         :   Prints this message'
            print '-s,--serial       :   Specifies the serial device to be used';
            print '-m,--send_msgtype :   Sends just one message of specified type.';
            print '                  :   Which current is 1 (TT) and 0 (RR).';
            sys.exit()
        elif opt in ("-s", "--serial"):
            serial_dev = arg
        elif opt in ("-m", "--send_msgtype"):
            try:
                ser = serial.Serial(serial_dev, DEFAULT_BAUDRATE)#, timeout=3)  
            except:
                print 'ERROR: ' + serial_dev + ' failed to initialize'
                print '...exiting'
                sys.exit()
            print 'Sending message of type ' + str(arg)
            send_CMD(ser,int(arg))
            ser.close();
            sys.exit();

    print 'Selected ', serial_dev,' as experiment 4 board'

    try:
        ser = serial.Serial(serial_dev, DEFAULT_BAUDRATE)#, timeout=3)  
    except:
        print 'ERROR: ' + serial_dev + ' failed to initialize'
        print '...exiting'
        sys.exit()

    ## Main Event Loop ##
    print 'BEGINNING OF COMMUNICATIONS'
    print '-------------------------------------------------------------'
    print ' -> This is where we would power on the board and wait 100ms' 
    
    # Initiate communication 
    send_CMD(ser,RR)
    
    while True:
        state = get_REPLY(ser)
        if (state == YY):
            send_CMD(ser,TT)
            ## TO BE CONTINUED
            # should probably jump to an image building function until the message is fully sent
            get_IMG(ser)
        elif (state == FF):
            print ' -> This is where we would power cycle' 
            send_CMD(ser,RR)
        elif (state == NN):
            time.sleep(100);
            send_CMD(ser,RR)
    ser.close();

###############################################################################
# SEND_CMD
#   This sends correctly formatted message to the experiment 4 board. As 
#   specified in the documentation, this can be one of two types: RR or TT
#   
#   Message Types:
#       (RR) Camera Ready?
#       (TT) Transmit Data Block
###############################################################################
def send_CMD(ser,msgtype):
    if (msgtype == TT):
        print 'sent (TT)'
        ser.write('TT') 
    elif (msgtype == RR):
        print 'sent (RR)'
        ser.write('RR') 

###############################################################################
# GET_REPLY
#   This gets the reply sent from the exp 4 board. The reply types are laid
#   out in more detail in the documentation mentioned at the top of this 
#   script. This function will BLOCK until a reply is received.
#   
#   Message Types:
#       (YY) Camera is ready
#       (NN) Camera is not ready
#       (FF) Camera has failed
###############################################################################
def get_REPLY(ser):
    rcvd = ser.read(2).decode("utf-8")

    print '(' + rcvd + ') received)' 
    if (rcvd == 'NN'):
        return NN
    elif (rcvd == 'YY'): 
        return YY
    elif (rcvd == 'FF'):
        return FF
    else :
        print "Reply (" + rcvd + ") not recognized"
        return -1

###############################################################################
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
###############################################################################
def get_IMG(ser):

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

if __name__ == "__main__":
    main(sys.argv[1:])
