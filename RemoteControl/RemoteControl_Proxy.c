// RemoteControl_Proxy.c

#include "RemoteControl_Proxy.h"

#define RC_BUFFER_MAX_LENGTH 2048


void 	RC_SendDataPackage( WebScreen_DataPackage *dataArrayParam, uint16_t dataCountParam )
{
	char outputStreamBuffer[ RC_BUFFER_MAX_LENGTH ];
	uint16_t packageLen = 0;
	memset( outputStreamBuffer, 0, RC_BUFFER_MAX_LENGTH );
	sprintf( outputStreamBuffer, "{%s:1408,%s:%d,%s:[", RC_APP_ID_TAG, RC_MSG_ID_TAG, RC_DATA_MSG, RC_DATA_ID_TAG );
	
	for( int k = 0; k < dataCountParam; k++ )
	{
		if( 1 == dataArrayParam[k].type ) // float 
		{
			sprintf( outputStreamBuffer, "%s{%s:%d,%s:%d,%s:%04.04lf}", outputStreamBuffer,
																				"\"id\"", dataArrayParam[ k ].id ,
																				"\"type\"", dataArrayParam[ k ].type,
																				"\"value\"",dataArrayParam[ k ].data.floatData
																				);
		}
		else if( 2 == dataArrayParam[k].type) // u64
		{
			sprintf( outputStreamBuffer, "%s{%s:%d,%s:%d,%s:%d}", outputStreamBuffer,
																				"\"id\"", dataArrayParam[ k ].id ,
																				"\"type\"", dataArrayParam[ k ].type,
																				"\"value\"",dataArrayParam[ k ].data.u64Data
																				);
		}
		else if( 3 == dataArrayParam[k].type)
		{

		}
		
		if( k < ( dataCountParam - 1) )
		{
			sprintf( outputStreamBuffer, "%s,",outputStreamBuffer);
		}
	}

	sprintf( outputStreamBuffer, "%s]}",outputStreamBuffer );

	RemoteControl_Broadcast( 0, outputStreamBuffer, strlen( outputStreamBuffer ) ); // ilk parametrenin bi onemi yok zaten
}

void 	RC_SendDeviceConfig( ApplicationConfiguration *appConfigStPtrParam )
{
	char outputStreamBuffer[ RC_BUFFER_MAX_LENGTH ];
	uint16_t packageLen = 0;
	memset( outputStreamBuffer, 0, RC_BUFFER_MAX_LENGTH );
	sprintf( outputStreamBuffer, "{%s:1408,%s:%d,%s:[", RC_APP_ID_TAG, RC_MSG_ID_TAG, RC_DEVICE_CONFIG_MSG, RC_DATA_ID_TAG );

	sprintf( outputStreamBuffer, "%s{\"id\":%d,\"value\":\"%s\"},", outputStreamBuffer, 0 , appConfigStPtrParam->serialPortName );
	sprintf( outputStreamBuffer, "%s{\"id\":%d,\"value\":\"%s\"},", outputStreamBuffer, 1 , appConfigStPtrParam->modbusTcpPortNumber );
	sprintf( outputStreamBuffer, "%s{\"id\":%d,\"value\":\"%s\"}]}", outputStreamBuffer, 2 , appConfigStPtrParam->ipAddress );
	printf("<RC_SendDeviceConfig> %s \n", outputStreamBuffer );
}