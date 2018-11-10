#include "SerialPortDriver.h"
#include <fcntl.h> // File control definitions


#define BAUDRATE B38400

int fd, c, res;
struct termios oldtio,newtio;
char buf[255];

SerialPortResultEnum     SerialPort_Begin ( const char *portName, speed_t baudRateParam, int dataBitsParam, int parityParam, int stopBitParam )
{
    SerialPortResultEnum retValueTemp = PORT_FAILED;
	/* Change /dev/ttyUSB0 to the one corresponding to your system */

	fd = open( portName ,O_RDWR | O_NOCTTY );	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */

    if(fd == -1)
    {
        #ifdef DEBUG_PRINTF
            printf("<SerialPort> -SerialPort_Begin- Error! in Opening %s  \n" ,portName);
        #endif
        /*kaanerrorfilelog*/
        return PORT_FAILED;
    }
	else
    {
        #ifdef DEBUG_PRINTF
            printf("<SerialPort> -SerialPort_Begin- %s Opened Successfully \n", portName);
        #endif
    }
    /*---------- Setting the Attributes of the serial port using termios structure --------- */

    struct termios SerialPortSettings;	/* Create the structure                          */

    tcgetattr( fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

    cfsetispeed( &SerialPortSettings, baudRateParam ); /* Set Read  Speed as 9600     */
    cfsetospeed( &SerialPortSettings, baudRateParam ); /* Set Write Speed as 9600     */
    
    SerialPortSettings.c_cflag &= ~CSIZE;    /* Clears the mask for setting the data size             */

    /* Set Parity */
    if( 1 == parityParam )
    {
        SerialPortSettings.c_cflag |= PARENB; 
    }
    else
    {
        SerialPortSettings.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity   */
    }

    /* Set Stop Bits */
    if( 2 == stopBitParam )
    {
        SerialPortSettings.c_cflag |= CSTOPB; /* CSTOPB = 2 Stop bits, */
    }
    else
    {
        SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB = it is cleared so 1 Stop bit */
    }
    /* set DataSize */ 
    if( 8 == dataBitsParam )
    {
        SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8     */
    }
    else if( 7 == dataBitsParam )
    {
        SerialPortSettings.c_cflag &=  ~CS8;      /* Set the data bits = 8     */
    }
    else
    {
        SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8     */
    }
      
    
	SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
	SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */

	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
	SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

	SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    /* Set the attributes to the termios structure*/
	if(( tcsetattr( fd, TCSANOW, &SerialPortSettings )) != 0)
    {
        #ifdef DEBUG_PRINTF
            printf("\n<SerialPort>  ERROR ! in Setting attributes");
        #endif
        return PORT_FAILED;
    }

	else
    {
        #ifdef DEBUG_PRINTF
            printf("\n<SerialPort>  BaudRate = %d  StopBits = %d  Parity   = %d \n", baudRateParam, stopBitParam, parityParam );
        #endif
    }

    return PORT_SUCCESS;
}

SerialPortResultEnum     SerialPort_Close( void )
{
    if ( fd > 0 )
    {
        close( fd );
    }
    return PORT_SUCCESS;
}

uint8_t SerialPort_Write( const char *inputBufferParam, uint8_t sizeParam )
{
    if ( fd < 0 )
    {
        printf("<SerialPort> -SerialPort_Write- SerialPort is not available..\n");
        /*kaanerrorfilelog*/
        return 0;
    }

	int  bytes_written  = 0;  	/* Value for storing the number of bytes written to the port */

	bytes_written = write(fd, inputBufferParam, sizeParam);/* use write() to send data to port */
    return 0;
}

uint16_t SerialPort_Read( uint8_t *readBufferParam, uint8_t byteCountParam , uint16_t timeoutParam )
{
    fd_set readSet;
    struct termios SerialPortSettings;	/* Create the structure                          */
    struct timeval timeout;
    tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer            */

    uint8_t read_buffer[255];   /* Buffer to store the data received              */
    int  bytes_read = 0;    /* Number of bytes read by the read() system call */
    int i = 0;

    timeout.tv_sec = 1;
    timeout.tv_usec = 500000;

	tcgetattr( fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

    if ( fd == -1 ) /* if port is not opened..*/
    {
        printf( "<SerialPort> -SerialPort_Read- ERROR ! Serial Port is not activated...\n" );
        /*kaanerrorfilelog*/
        return 0;
    }

	SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
	SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


	SerialPortSettings.c_iflag &= ~( IXON | IXOFF | IXANY );          /* Disable XON/XOFF flow control both i/p and o/p */
	SerialPortSettings.c_iflag &= ~( ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

	SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/
    SerialPortSettings.c_lflag = 0;
	/* Setting Time outs */
	SerialPortSettings.c_cc[ VMIN ] = byteCountParam; /* Read at least 10 characters */
	SerialPortSettings.c_cc[ VTIME ] = timeoutParam; /* Wait indefinetly   */


    /* Set the attributes to the termios structure*/
	if( ( tcsetattr( fd, TCSANOW, &SerialPortSettings ) ) != 0 )
    {
        printf( "<SerialPort>  -SerialPort_Read- ERROR ! in Setting attributes \n" );
        /*kaanerrorfilelog*/
        return 0;
    }

    /* timeout ayarlamalari */
    FD_ZERO( &readSet ); /* clear the set */
    FD_SET( fd, &readSet ); /* add our file descriptor to the set */
    i = select( fd + 1, &readSet, NULL, NULL, &timeout);
    if( i == -1 )
    {
        printf( "<SerialPort> -SerialPort_Read- select error .. \n" );
        return 0;
    }
    else if ( i == 0 )
    {
        printf("<SerialPort> -SerialPort_Read- timeout error...\n");
        return 0;
    }
    else
    {
        bytes_read = read( fd, &read_buffer, 255 ); /* Read the data                   */ 
    }
	if ( bytes_read != -1 )
    {
        #ifdef DEBUG_PRINTF
        printf("\n  Bytes Rxed Count : %d, Bytes : ", bytes_read ); /* Print the number of bytes read */
        
        /*printing only the received characters*/
        for( i = 0; i < bytes_read ; i++ )
        {
            printf( "%d ",read_buffer[i]);
        }
        printf("\n  ");
        #endif

        memcpy( readBufferParam, read_buffer, bytes_read );
        return bytes_read;
    }
    else
    {
        return 0;
    }
}

/* kaanbak paramlar int yerine enum olsa daha dadlu olur */
int SerialPort_SetAttributes( speed_t speedParam, int parityParam, int stopBitsParam, int dataBitsParam )
{
    struct termios tty;
    printf("<SerialPort_ ### SET ATTIRBUTES ### \n");
    if( tcgetattr( fd, &tty ) < 0 ) 
    {
        printf(" <SerialPort> -SerialPort_SetAttributes- ERROR from tcgetattr: \n" );
        //printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed( &tty, (speed_t) speedParam );
    cfsetispeed( &tty, (speed_t) speedParam );

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag |= CSTOPB;     /* only need 2 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("<SerialPort> -SerialPort_SetAttributes- ERROR from tcsetattr: \n");
        //printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
