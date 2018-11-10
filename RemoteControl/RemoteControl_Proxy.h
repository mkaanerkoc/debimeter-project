//RemoteControl_Proxy.h

#ifndef REMOTE_CONTROL_PROXY_H
#define REMOTE_CONTROL_PROXY_H

#include <stdint.h>
#include "RemoteControlManager.h"
#include "../ConfigurationManager.h"

typedef enum WebScreen_DataTypes
{
	INTEGER_TYPE_DT = 0,
	FLOAT_TYPE_DT = 1,
	U_INTEGER_TYPE_DT = 2
}WebScreen_DataTypes;

typedef union WebScreen_Data
{
	int intData;
	float floatData;
	unsigned int uintData;
	uint16_t 	u16Data;
	uint64_t 	u64Data;
}WebScreen_Data;

typedef struct WebScreen_DataPackage
{
	uint8_t id;
	WebScreen_Data data;
	WebScreen_DataTypes type;
}WebScreen_DataPackage;



extern void 	RC_SendDataPackage( WebScreen_DataPackage *dataArrayParam, uint16_t dataCountParam );
extern void 	RC_SendDeviceConfig( ApplicationConfiguration *appConfigStPtrParam );

void RC_ParseDataPackage( const char *jsonParam, WebScreen_DataPackage *dataArrayParam, uint16_t *dataCountParam );

#endif
