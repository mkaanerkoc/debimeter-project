#include "IPManager.h"


#define MAX_LINE_LENGTH 128

// private functions 

// public functions 


// private variables
FILE *configFilePtr = NULL;

bool openConfigFileForRead( const char *confFileNameWithPathParam );
bool openConfigFileForModify( const char *confFileNameWithPathParam );
bool closeConfigFile( void );
bool seekConfiguration( void );
void getCurrentConfigurationsInfo( void );
void removeDisabledSettings( void );

bool isLineEnabled( const char *lineBufferParam );

const char *configFileName = "/etc/dhcpcd.conf";
struct ifaddrs *ifaddr, *ifa;


typedef struct InterfaceConfigStruct
{
	int id;
}InterfaceConfigStruct;

typedef struct DHCPSettingsInfo
{
	int activeInterface;
}DHCPSettingsInfo;


void IPManager_PrintIpAddress( void )
{
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) 
    {
        printf("<IPManager> PrintIpAddress() - getifaddrs error ! \n");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;  

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if((strcmp(ifa->ifa_name,"eth0")==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("<IPManager> PrintIpAddress() - getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("<IPManager> PrintIpAddress() - \tInterface : <%s>\n",ifa->ifa_name );
            printf("<IPManager> PrintIpAddress() - \tAddress : <%s>\n", host); 
        }
    }
    freeifaddrs(ifaddr);
    return;
}

void IPManager_GetIpAddress( char *hostNameParam )
{
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) 
    {
        printf("<IPManager> GetIpAddress() - getifaddrs error ! \n");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;  

        s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),hostNameParam, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if( (strcmp(ifa->ifa_name,"eth0") ==0 ) && ( ifa->ifa_addr->sa_family==AF_INET) )
        {
            if (s != 0)
            {
                printf("<IPManager> GetIpAddress() - getnameinfo() failed: %s\n", gai_strerror(s));
                return;
            }
        }
    }
    freeifaddrs(ifaddr);
    return;
}

bool isStaticIpConfigured( void )
{
	bool retValueTemp = false;
	return retValueTemp;
}

bool openConfigFileForRead( const char *confFileNameWithPathParam )
{
	bool retValueTemp = false;
	if( ( configFilePtr = fopen( confFileNameWithPathParam, "r") ) != NULL )
	{
		retValueTemp = true;
	} 
	else 
	{
	    /* error processing, couldn't open file */
		retValueTemp = false;
	}
	return retValueTemp;
}

bool openConfigFileForModify( const char *confFileNameWithPathParam )
{
	bool retValueTemp = false;
	if( ( configFilePtr = fopen( confFileNameWithPathParam, "w") ) != NULL )
	{
		retValueTemp = true;
	} 
	else 
	{
	    /* error processing, couldn't open file */
		retValueTemp = false;
	}
	return retValueTemp;
}

bool closeConfigFile( void )
{
	//fflush( configFilePtr );
	if( NULL == configFilePtr )
	{
		printf("<closeConfigFile> configFilePtr is NULL \n" );
		return false;
	}
	fclose( configFilePtr );
	return true;
}

bool seekConfiguration( void )
{
	int lineCount = 0;
	char lineBuffer[ MAX_LINE_LENGTH ];
	fseek( configFilePtr , 0 , SEEK_SET );
	char *settingsFoundPtr = NULL;
	bool isLineEnabledTemp = false;
	while( fgets( lineBuffer, MAX_LINE_LENGTH, configFilePtr ) != NULL )
	{
        /* then no read error */
        lineCount +=1;
        isLineEnabledTemp = isLineEnabled( lineBuffer );
        if( false == isLineEnabledTemp )
        {
        	continue;
        }
        if( strstr( lineBuffer, "static ip_address" ) != NULL) 
        {
    		printf("E : %d: %s",lineCount, lineBuffer );	
        }
        if( strstr( lineBuffer, "static routers" ) != NULL) 
        {
        	printf("E : %d: %s",lineCount, lineBuffer );	
    	}
        if( strstr( lineBuffer, "static routers" ) != NULL) 
        {
       		printf("E : %d: %s",lineCount, lineBuffer );	
    	}
    }
    fseek( configFilePtr , 0 , SEEK_SET );
    return true;
}

void getCurrentConfigurationsInfo( void )
{
	int lineCount = 0;
	char lineBuffer[ MAX_LINE_LENGTH ];
	char *settingsFoundPtr = NULL;
	bool isLineEnabledTemp = false;
	uint8_t totalInterfaceLineCount  = 0;
	uint8_t totalStaticIpLineCount = 0;
	uint8_t totalRoutersLineCount = 0;
	uint8_t totalDnsServerLineCount = 0;

	fseek( configFilePtr , 0 , SEEK_SET );
	while( fgets( lineBuffer, MAX_LINE_LENGTH, configFilePtr ) != NULL )
	{
        lineCount +=1;
        isLineEnabledTemp = isLineEnabled( lineBuffer );
        if( false == isLineEnabledTemp )
        {
        	continue;
        }
        if( ( settingsFoundPtr = strstr( lineBuffer, "interface " ) ) != NULL) 
        {
    		if( true == isLineEnabledTemp )
    		{
    			printf("E : %d: %s",lineCount, lineBuffer );	
    			totalInterfaceLineCount += 1;
    		}
        }
        if( strstr( lineBuffer, "static ip_address" ) != NULL) 
        {
    		printf("E : %d: %s",lineCount, lineBuffer );	
    		totalStaticIpLineCount += 1;
        }
        if( strstr( lineBuffer, "static routers" ) != NULL) 
        {
        	printf("E : %d: %s",lineCount, lineBuffer );
    		totalRoutersLineCount += 1;	
        }
        if( strstr( lineBuffer, "static domain_name_servers" ) != NULL) 
        {
        	printf("E : %d: %s",lineCount, lineBuffer );	
    		totalDnsServerLineCount += 1;
        }
    } // end of  while( fgets( lineBuffer, MAX_LINE_LENGTH, configFilePtr ) != NULL )
    printf(" Total Interface Line Count : %d \n", totalInterfaceLineCount );
    printf(" Total Static IP Line Count : %d \n", totalStaticIpLineCount );
    printf(" Total Routers Line Count : %d \n", totalRoutersLineCount );
    printf(" Total DNS Server Line Count : %d \n", totalDnsServerLineCount );
	fseek( configFilePtr , 0 , SEEK_SET );
    return;
}

void removeDisabledSettings( void )
{
	int lineCount = 0;
	char lineBuffer[ MAX_LINE_LENGTH ];
	fseek( configFilePtr , 0 , SEEK_SET );
	char *settingsFoundPtr = NULL;
	bool isLineEnabledTemp = false;
	while( fgets( lineBuffer, MAX_LINE_LENGTH, configFilePtr ) != NULL )
	{
        /* then no read error */
        lineCount += 1;
        isLineEnabledTemp = isLineEnabled( lineBuffer );
        if( true == isLineEnabledTemp )
        {
        	continue;
        }
        if( strstr( lineBuffer, "static ip_address" ) != NULL) 
        {
    		printf("D : %d: %s",lineCount, lineBuffer );	
        }
        if( strstr( lineBuffer, "static routers" ) != NULL) 
        {
        	printf("D : %d: %s",lineCount, lineBuffer );	
    	}
        if( strstr( lineBuffer, "static routers" ) != NULL) 
        {
       		printf("D : %d: %s",lineCount, lineBuffer );	
    	}
    }
    fseek( configFilePtr , 0 , SEEK_SET );
    return true;
}

bool isLineEnabled( const char *lineBufferParam )
{
	if( NULL == lineBufferParam )
	{
		return false;
	}
	if( '#' == lineBufferParam[ 0 ] )
	{
		return false;
	}
	else
	{
		return true;
	}
}