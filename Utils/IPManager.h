#ifndef IP_MANAGER_H
#define IP_MANAGER_H


#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

void IPManager_PrintIpAddress( void );
void IPManager_GetIpAddress( char *hostNameParam );

#endif