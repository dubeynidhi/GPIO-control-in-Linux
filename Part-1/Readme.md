REPOSITORY CONTENTS:

-> RGBLed.c (Program Source code)
-> Makefile (Makefile for the Sourcecode)
-> README

_________________________________________________________________________________________________________________________________

ABOUT: 

This project demonstrates the IO control through digital GPIO(General Purpose Input Output) pins of the Galileo board
by controlling a LED to run a seven step sequence of RGB color combinations and the luminous intensity. Variation of 
intensity is achieved by PWM(Pulse Width Modulation) based on the duty cycle given by the user. Mouse activity is 
monitored using a thread and when a mouse click event occurs, the sequence terminates after completing the step it is executing.
_________________________________________________________________________________________________________________________________

SYSTEM REQUIREMENTS:

-> LED 
-> LINUX KERNEL : Minimum version of 2.6.19 is expected.
-> SDK: iot-devkit-glibc-x86_64-image-full-i586-toolchain-1.7.2
-> GCC: Minimum version of 4.8 is required to run -pthread option while compiling.
-> Intel Galileo Gen2
-> USB mouse
_________________________________________________________________________________________________________________________________

SETUP:

->LED should be connected to the GPIO pins and the GND pin should be grounded properly by connecting to the GND of Galileo board
-> Must boot Galileo from SD card with linux version 3.19.
-> Set up the board using setup guide.
-> Connect the USB mouse to the Galileo board
_________________________________________________________________________________________________________________________________

COMPILATION:

-> On the host, open directory in terminal in which files are present and type make.
-> type sudo screen /dev/ttyUSB0 115200 to communicate with board.
-> type ifconfig enp0s20f6 192.168.1.5 netmask 255.255.0.0 up
-> Open a new terminal using scp copy your RGBLed executable file to desired location on board.
-> Now use ssh root@192.168.1.5 and navigate to the folder where the executable file is present.
_________________________________________________________________________________________________________________________________

EXECUTION:

->type ./RGBLed to run the code
_________________________________________________________________________________________________________________________________
EXPECTED OUTPUT:

-> On running the executable, you must enter the values of duty cycle and the pin numbers connected when asked. 
-> You will see the LED blinking with the colors assigned in the seven step sequence. The intensity will vary according 
   to the duty cycle specified by you. The duty cycle is should be within the range of 0 and 100.
-> When a mouse is clicked, the LED stops blinking and the program terminates.
_________________________________________________________________________________________________________________________________
