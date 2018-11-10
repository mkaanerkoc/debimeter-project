#ifndef MODBUS_TCP_SLAVE_H
#define MODBUS_TCP_SLAVE_H



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

#include "ModbusCore.h"
#include "ModbusSlave_RegisterMap.h"

#define  MODBUS_TCP_SLAVE_BUFFER_LENGTH 	1024


//Globals
pthread_t 		modbusTcpTask;
struct 			mg_mgr 	modbusTcpServerMgr;

uint16_t 		_lastReceivedPackageID_ModbusTCP;

uint16_t 		modbusTcpPDULength;
uint8_t  		modbusTcpPDUBuffer[MODBUS_TCP_SLAVE_BUFFER_LENGTH];

//Function Prototypes
extern void 	Initialize_ModbusTCP( const char *portNumberParam );
static void 	modbusTCP_EventHandler(struct mg_connection *nc, int ev, void *p);

void 			ModbusTCP_ParsePackage(uint8_t *data,uint16_t len,struct mg_connection *nc);

//Task Prototypes
void 			*ModbusTCP_Task(void *arg);


#endif
