#include "RemoteControlManager.h"
#include "RemoteControl_Proxy.h"

//config
const char      *rcPort = "1233";

//locals
pthread_t 		rcManagerTask;
uint8_t         connectedClientCount;
struct 			mg_mgr 	remoteControlTcpManager;
struct          mg_connection *nc;
struct 			mg_connection *lastConnectedClient;
void	        *RemoteControl_Task(void *arg);

static void remoteControl_EventHandler(struct mg_connection *nc, int ev, void *ev_data);
static void remoteControl_BroadcastEventHandler(struct mg_connection *nc, int ev, void *ev_data);
static int  is_websocket(const struct mg_connection *nc);


bool getStringBetweenTwoChar( const char *stringParam, char startCharParam, char stopCharParam, char *outputValue ); // bunu buradan common ' a tasiyabiliriz..
bool getIntegerBetweenTwoChar( const char *stringParam, char startCharParam, char stopCharParam, int *outputValue );
bool getFloatBetweenTwoChar( const char *stringParam, char startCharParam, char stopCharParam, float *outputValue );


// externs 
extern totalVolumeManager totalVm;

void RemoteControl_Initialize( void )
{
    connectedClientCount = 0;
    mg_mgr_init( &remoteControlTcpManager, NULL );
    nc = mg_bind( &remoteControlTcpManager, rcPort, remoteControl_EventHandler );
    mg_set_protocol_http_websocket( nc );
    pthread_create( &rcManagerTask, NULL, RemoteControl_Task, NULL );
}


void RemoteControl_Spawn(void)
{
  	pthread_join( rcManagerTask, NULL );
}

void	*RemoteControl_Task(void *arg)
{
    printf("<RemoteControl> Task Created ! \n");
    while(1)
    {
        mg_mgr_poll(&remoteControlTcpManager, 200 );
    }
    mg_mgr_free(&remoteControlTcpManager);
}

static void remoteControl_EventHandler(struct mg_connection *nc, int ev, void *ev_data)
{
    struct mbuf *io = &nc->recv_mbuf;
    lastConnectedClient = nc;
    uint16_t messageType = 0;
    uint16_t messageLength = 0;
    switch (ev)
    {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
            /* New websocket connection. Tell everybody. */
            //broadcast(nc, mg_mk_str("++ joined"));
            connectedClientCount++;
            printf("<RemoteControl_Event> New client joined. Total client count : %d \n", connectedClientCount );
            RC_SendDeviceConfig( &appConfigSt );
            break;
        }
        case MG_EV_WEBSOCKET_FRAME:
        {
            struct websocket_message *wm = (struct websocket_message *) ev_data;
            /* New websocket message. Tell everybody. */
            struct mg_str d = {(char *) wm->data, wm->size };
            //broadcast(nc, d);
            //printf("<RemoteControl_Event> New WebSocketFrame received. Length : %d\n", wm->size );
            if( wm->size >= MINIMUM_RC_MSG_LENGTH )
            {
                RemoteControl_ParseReceivedPackage( (char *) wm->data, wm->size );
            }
            break;
        }
        case MG_EV_HTTP_REQUEST:
        {
            //mg_serve_http(nc, (struct http_message *) ev_data, s_http_server_opts);
            break;
        }
        case MG_EV_CLOSE:
        {
            /* Disconnect. Tell everybody. */
            if( is_websocket( nc ) )
            {
                connectedClientCount--;
                printf( "<RemoteControl_Event> A Client disconnected.Total client count : %d \n", connectedClientCount );
                //broadcast(nc, mg_mk_str("-- left"));
            }
            break;
        }
    }
}

void RemoteControl_Broadcast(uint16_t msgID, uint8_t *data, uint16_t length )
{
    uint8_t outputBuffer[ length + 2 ];
    memcpy( outputBuffer+2, data, length );
    length = length + 2;
    outputBuffer[0] = (uint8_t)( length >> 8 );
    outputBuffer[1] = (uint8_t)( length & 0xFF );
    
    #if 0 // bunlar eski uzak komuta parcalari biz webSocket'de JSON takiliyoruz hep...
	
	uint16_t tempLength = length + 10;
	outputBuffer[0] = (uint8_t)(tempLength >> 8 );
	outputBuffer[1] = (uint8_t)(tempLength & 0xFF );
	outputBuffer[2] = COMMAND_STARTER_1;
	outputBuffer[3] = (uint8_t)(msgID >> 8);
	outputBuffer[4] = (uint8_t)(msgID & 0xFF);
	outputBuffer[5] = (uint8_t)(length >> 8);
	outputBuffer[6] = (uint8_t)(length & 0xFF);
	
	//kaanbak , buraya CRC hesaplanmasi gelecek...
	outputBuffer[length+7] = 0x23;
	outputBuffer[length+8] = 0x24;
	outputBuffer[length+9] = COMMAND_FINISHER_1;
    #endif
    /*printf("<RemoteControl_Broadcast> Data : \n");
    for(int k = 0 ; k < length ; k ++ )
    {
        printf("%c",data[k]);
    }
    printf("\n");*/
	mg_broadcast( &remoteControlTcpManager, remoteControl_BroadcastEventHandler, outputBuffer, length );
}

static void remoteControl_BroadcastEventHandler(struct mg_connection *nc, int ev, void *ev_data)
{
	(void) ev;
	char s[32];
    char addr[32];
	char *ev_data_temp = (char*)ev_data;
	uint16_t tempLength = (ev_data_temp[0] << 8 ) | ( ev_data_temp[1]);
	struct mg_connection *c;
    //printf("<remoteControl_BroadcastEventHandler> SizeOf Data : %d , Data : %s \n", tempLength, ( ev_data_temp + 2 ) );
    mg_sock_addr_to_str( &nc->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT );
	for( c = mg_next( nc->mgr, NULL ); c != NULL; c = mg_next( nc->mgr, c) )
	{
        if ( c == nc )
        {
            continue;
        }
		mg_send_websocket_frame( c, WEBSOCKET_OP_TEXT, ev_data_temp + 2, ( tempLength -2 ) );
	}
}

static int is_websocket(const struct mg_connection *nc)
{
    return nc->flags & MG_F_IS_WEBSOCKET;
}


int RemoteControl_ParseReceivedPackage( const char *data, uint16_t length )
{
    char    *jsonParserPtr = NULL;
    char   parserPiece[ 128 ];
    strncpy( parserPiece, data , MINIMUM_RC_MSG_LENGTH + 6 );
    printf("<RemoteControl_ParseReceivedPackage> Received Data Bytes : %s \n", parserPiece );
    if( ( jsonParserPtr = strstr( data, "appKeyId" ) ) != NULL )
    {
        int appKeyId = 0x00;
        if( true == getIntegerBetweenTwoChar( jsonParserPtr, ':', ',', &appKeyId ) )
        {
            if( 1408 != appKeyId )
            {
                printf("<RemoteControl_ParseReceivedPackage> Wrong App ID  %d\n", appKeyId );
                return;
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        printf("<RemoteControl_ParseReceivedPackage> AppKeyId could not found \n" );
        return;
    }
    if( ( jsonParserPtr = strstr( jsonParserPtr, "messageId" ) ) != NULL )
    {
        int messageId = 0x00;
        if( true == getIntegerBetweenTwoChar( jsonParserPtr, ':', '}', &messageId ) )
        {
           	printf("<RemoteControl_ParseReceivedPackage> Received Msg ID is : %d \n", messageId );
            if( 17 == messageId )
            {
                totalVm.ResetValue( &totalVm );
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

bool getStringBetweenTwoChar( const char *stringParam, char startCharParam, char stopCharParam, char *outputValue )
{
    char *ptr1 = strchr( stringParam, startCharParam );
    char *ptr2 = strchr( stringParam, stopCharParam );
    if ( ( ptr1 == NULL ) || ( ptr2 == NULL ) )
    {
        printf("<getStringBetweenTwoChar> start, stop chars could not found \n");
        return false;
    }
    ptr1 = ptr1 + 1;

    int indexDiff = ptr2 - ptr1;
    if( indexDiff <= 0 )
    {
        printf("<getStringBetweenTwoChar>  stop char could not exist before start char \n");
        return false;
    }
    if( indexDiff >= 64 )
    {
        printf("<getStringBetweenTwoChar>  the substring length cannot exceed 64 char \n");
        return false;
    }
    strncpy( outputValue, ptr1, indexDiff );
    return true;
}

bool getIntegerBetweenTwoChar( const char *stringParam, char startCharParam, char stopCharParam , int *outputValue )
{
    char outputBuffer[ 64 ];
    char *ptr1 = strchr( stringParam, startCharParam );
    char *ptr2 = strchr( stringParam, stopCharParam );
    if ( ( ptr1 == NULL ) || ( ptr2 == NULL ) )
    {
        printf("<getIntegerBetweenTwoChar> start, stop chars could not found \n");
        return false;
    }
    ptr1 = ptr1 + 1;

    int indexDiff = ptr2 - ptr1;
    if( indexDiff <= 0 )
    {
        printf("<getIntegerBetweenTwoChar>  stop char could not exist before start char \n");
        return false;
    }
    if( indexDiff >= 64 )
    {
        printf("<getIntegerBetweenTwoChar>  the substring length cannot exceed 64 char \n");
        return false;
    }
    memset( outputBuffer, 0, sizeof( outputBuffer ) );
    strncpy( outputBuffer, ptr1, indexDiff );
    sscanf( outputBuffer, "%d", outputValue );
    return true;
}

bool getFloatBetweenTwoChar( const char *stringParam, char startCharParam, char stopCharParam, float *outputValue )
{
    char outputBuffer[ 64 ];
    char *ptr1 = strchr( stringParam, startCharParam );
    char *ptr2 = strchr( stringParam, stopCharParam );
    if ( ( ptr1 == NULL ) || ( ptr2 == NULL ) )
    {
        printf("<getFloatBetweenTwoChar> start, stop chars could not found \n");
        return false;
    }
    ptr1 = ptr1 + 1;

    int indexDiff = ptr2 - ptr1;
    if( indexDiff <= 0 )
    {
        printf("<getFloatBetweenTwoChar>  stop char could not exist before start char \n");
        return false;
    }
    if( indexDiff >= 64 )
    {
        printf("<getFloatBetweenTwoChar>  the substring length cannot exceed 64 char \n");
        return false;
    }
    memset( outputBuffer, 0, sizeof( outputBuffer ) );
    strncpy( outputBuffer, ptr1, indexDiff );
    sscanf( outputBuffer, "%3.5f", outputValue );
    return true;
}
