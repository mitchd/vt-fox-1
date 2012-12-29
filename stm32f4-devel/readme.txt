(Original README contents are below)


This is the beginning of Virginia Tech's AMSAT FOX-1 Camera development.

We're beginning with the ChibiOS/RT demo on the STM32F4-DISCOVERY board to
start working toward implementing our code onto an STM32L151 processor.

We need a combination of STM32F4 and STM32L1 Discovery boards to ensure that
our code is portable.

This project is based off ChibiOS/RT and jpegant
http://www.chibios.org/dokuwiki/doku.php
http://code.google.com/p/jpegant/

ChibiOS/RT is licensed under the GPLv3 license
jpegant is licensed under the GPLv2 license

We have a licensing problem it looks like.






*****************************************************************************
** ChibiOS/RT port for ARM-Cortex-M4 STM32F407.                            **
*****************************************************************************

** TARGET **

The demo runs on an ST STM32F4-Discovery board.

** The Demo **

The demo shows how to use the ADC, PWM and SPI drivers using asynchronous
APIs. The ADC samples two channels (temperature sensor and PC1) and modulates
the PWM using the sampled values. The sample data is also transmitted using
the SPI port 2 (NSS=PB12, SCK=PB13, MISO=PB14, MOSI=PB15).
By pressing the button located on the board the test procedure is activated
with output on the serial port SD2 (USART2).

** Build Procedure **

The demo has been tested by using the free Codesourcery GCC-based toolchain
and YAGARTO. just modify the TRGT line in the makefile in order to use
different GCC toolchains.

** Notes **

Some files used by the demo are not part of ChibiOS/RT but are copyright of
ST Microelectronics and are licensed under a different license.
Also note that not all the files present in the ST library are distributed
with ChibiOS/RT, you can find the whole library on the ST web site:

                             http://www.st.com
