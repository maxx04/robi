#include "SerialPort_nano.h"


using namespace std;

SerialPort::SerialPort(const char *portName)
{

	//portName = uart_target;	  //TODO port

	//int ii, jj, kk;

	// SETUP SERIAL WORLD
	fd = -1;
	struct termios  port_options;   // Create the structure                          

	tcgetattr(fd, &port_options);	// Get the current attributes of the Serial port 
   
	//------------------------------------------------
	//  OPEN THE UART
	//------------------------------------------------
	// The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR   - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//	    O_NDELAY / O_NONBLOCK (same function) 
	//               - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//                 if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//				   immediately with a failure status if the output can't be written immediately.
	//                 Caution: VMIN and VTIME flags are ignored if O_NONBLOCK flag is set.
	//	    O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.fid = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode

	fd = open(portName, O_RDWR | O_NOCTTY);

	tcflush(fd, TCIFLUSH);
	tcflush(fd, TCIOFLUSH);

	usleep(1000000);  // 1 sec delay

	this->connected = false;

	if (fd == -1)
	{
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		
	}

	//------------------------------------------------
	// CONFIGURE THE UART
	//------------------------------------------------
	// flags defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html
	//	Baud rate:
	//         - B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, 
	//           B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, 
	//           B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE: - CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD  - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL  - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)

	port_options.c_cflag &= ~PARENB;            // Disables the Parity Enable bit(PARENB),So No Parity   
	port_options.c_cflag &= ~CSTOPB;            // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit 
	port_options.c_cflag &= ~CSIZE;	            // Clears the mask for setting the data size             
	port_options.c_cflag |= CS8;               // Set the data bits = 8                                 	 
	port_options.c_cflag &= ~CRTSCTS;           // No Hardware flow Control                         
	port_options.c_cflag |= CREAD | CLOCAL;                  // Enable receiver,Ignore Modem Control lines       				
	port_options.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable XON/XOFF flow control both input & output
	port_options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode                            
	port_options.c_oflag &= ~OPOST;                           // No Output Processing

	port_options.c_lflag = 0;               //  enable raw input instead of canonical,

	port_options.c_cc[VMIN] = VMINX;       // Read at least 1 character
	port_options.c_cc[VTIME] = 0;           // Wait indefinetly 

	cfsetispeed(&port_options, BAUDRATE);    // Set Read  Speed 
	cfsetospeed(&port_options, BAUDRATE);    // Set Write Speed 

	// Set the attributes to the termios structure
	int att = tcsetattr(fd, TCSANOW, &port_options);

	if (att != 0)
	{
		printf("\nERROR in Setting port attributes");
	}
	else
	{
		this->connected = true;
		printf("\nSERIAL Port ready.\n");
	}

	// Flush Buffers
	tcflush(fd, TCIFLUSH);
	tcflush(fd, TCIOFLUSH);

	usleep(500000);   // 0.5 sec delay
 
	
}

SerialPort::~SerialPort()
{
//-------------------------------------------
//  CLOSE THE SERIAL PORT
//-------------------------------------------
	if (this->connected) 
	{
		this->connected = false;
		close(fd);
	}
}							    

int SerialPort::readSerialPort(char *buffer, unsigned int buf_size)
{
	//int num = 0;

	//while (serialDataAvail(fd) > 0 || num >= MAX_DATA_LENGTH)
	//	buffer[num++] = (char)serialGetchar(fd);



//--------------------------------------------------------------
// RECEIVING BYTES - AND BUILD MESSAGE RECEIVED
//--------------------------------------------------------------
	unsigned char rx_buffer[VMINX];
	unsigned char serial_message[NSERIAL_CHAR];
	bool          pickup = true;
	ssize_t       rx_length;
	int           nread = 0;

	//tcflush(fd, TCIOFLUSH);

	//usleep(1000);   // .001 sec delay

	//printf("Ready to receive message.\n");

	int ii;

	for (ii = 0; ii < NSERIAL_CHAR; ii++)  serial_message[ii] = ' ';

	nread = 0;


	while (pickup && fd != -1)
	{
		nread++;

		rx_length = read(fd, (void*)rx_buffer, VMINX);   // Filestream, buffer to store in, number of bytes to read (max)

		//printf("Event %d, rx_length=%d, Read=%s\n", nread, rx_length, rx_buffer);

		if (rx_length < 0)
		{
			//An error occured (will occur if there are no bytes)
		}

		if (rx_length == 0)
		{
			//No data waiting
		}

		if (rx_length >= 0)
		{
			if (nread <= NSERIAL_CHAR)   serial_message[nread - 1] = rx_buffer[0];   // Build message 1 character at a time

			if (rx_buffer[0] == '\n')   pickup = false; // \n symbol is terminator

			if (nread > NSERIAL_CHAR) return -1;
		}
	}

	//printf("\nMessage Received: %s", serial_message);

	return nread;
}

bool SerialPort::writeSerialPort(char *buffer, unsigned int buf_size)
{
	//--------------------------------------------------------------
	// TRANSMITTING BYTES
	//--------------------------------------------------------------

	//printf("fd 1 = %d\n", fd);

	if (fd != -1)
	{
		ssize_t count = write(fd, buffer, buf_size);		//Filestream, bytes to write, number of bytes to write

		//tcflush(fd, TCIOFLUSH);

		//usleep(1000);   // .001 sec delay

		//printf("Count = %d\n", count);

		if (count < 0)
		{
			printf("UART TX error\n");
			return false;
		}

	}

	//usleep(1000000);  // 1 sec delay

	return true;
}

bool SerialPort::writeSerialPort(const char *buffer)
{
	this->writeSerialPort((char*)buffer, (unsigned int)strlen(buffer));

	return true;
}

bool SerialPort::isConnected()
{
	return this->connected;
}


