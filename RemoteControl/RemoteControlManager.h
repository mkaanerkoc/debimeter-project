#ifndef REMOTE_CONTROL_MANAGER_H
#define REMOTE_CONTROL_MANAGER_H

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mongoose.h>
#include <stdbool.h>
#include "CommonTypeDefs.h"
#include "TotalVolumeCalculator.h"


#define MINIMUM_RC_MSG_LENGTH    		31

#define COMMAND_STARTER_1   			0x03
#define COMMAND_FINISHER_1  			0x04

#define RC_APP_ID_TAG       			"\"appKeyId\""
#define RC_MSG_ID_TAG 					"\"messageId\""
#define RC_DATA_ID_TAG 					"\"data\""

typedef enum RC_MessageId
{
	RC_AUTH_MSG 			= 0x01,
	RC_DATA_MSG 			= 0x02,
	RC_DIAGNOSTICS_MSG 		= 0x03,
	RC_ACK_MSG 				= 0X04,
	RC_DEVICE_CONFIG_MSG	= 0x05
}RC_MessageId;

extern void RemoteControl_Initialize    ( void );
extern void RemoteControl_Spawn         ( void );
extern void RemoteControl_Broadcast     ( uint16_t msgID, uint8_t *data, uint16_t length );


extern int RemoteControl_ParseReceivedPackage( const char *data, uint16_t length );


#endif // #ifndef REMOTE_CONTROL_MANAGER_H
