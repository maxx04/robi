#ifndef SERIALPORT_H
#define SERIALPORT_H

#define MAX_DATA_LENGTH 255

// Define Constants

#define     NSERIAL_CHAR   MAX_DATA_LENGTH
#define     VMINX          1
#define     BAUDRATE       B9600

#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>       // Used for UART
#include <sys/fcntl.h>    // Used for UART
#include <termios.h>      // Used for UART
#include <string.h>

class SerialPort
{
private:
	//const char* uart_target = "/dev/ttyTHS1";
	bool connected;
	int fd;

public:

	SerialPort(const char *portName);
	~SerialPort();

	int readSerialPort(char *buffer, unsigned int buf_size = MAX_DATA_LENGTH);
	bool writeSerialPort(char *buffer, unsigned int buf_size);
	bool writeSerialPort(const char *buffer);
	bool isConnected();
};

#endif 
