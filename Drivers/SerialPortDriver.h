#ifndef SERIAL_PORT_DRIVER_H
#define SERIAL_PORT_DRIVER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

typedef enum SerialPortResultEnum
{
    PORT_SUCCESS,
    PORT_FAILED
}SerialPortResultEnum;

extern SerialPortResultEnum     SerialPort_Begin 		( const char *portName, speed_t baudRateParam, int dataBitsParam, int parityParam, int stopBitParam );
extern uint8_t                  SerialPort_Write        ( const char *inputBufferParam, uint8_t sizeParam );
extern uint16_t                 SerialPort_Read         ( uint8_t *readBufferParam, uint8_t byteCountParam , uint16_t timeoutParam );
extern SerialPortResultEnum     SerialPort_Close        ( void );
extern int                      SerialPort_SetAttributes( speed_t speedParam, int parityParam, int stopBitsParam, int dataBitsParam );
#endif // #ifndef SERIAL_PORT_DRIVER_H
