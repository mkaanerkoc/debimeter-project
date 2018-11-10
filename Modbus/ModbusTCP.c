#include "ModbusTCP.h"


//Constants

const char *modbus_tcp_port = "502";

void 			Initialize_ModbusTCP(void)
{
	mg_mgr_init(&modbusTcpServerMgr, NULL);
	mg_bind(&modbusTcpServerMgr, modbus_tcp_port, modbusTCP_EventHandler);
	pthread_create(&modbusTcpTask,NULL,ModbusTCP_Task,NULL);
}


static void 	modbusTCP_EventHandler(struct mg_connection *nc, int ev, void *p)
{
	struct mbuf *io = &nc->recv_mbuf;
	uint16_t messageType = 0;
	uint16_t messageLength = 0;
	(void) p;
	switch (ev) 
	{
		case MG_EV_ACCEPT:
			printf("MODBUS_TCP_MG_EV_ACCEPT \n");
			break;
		case MG_EV_CLOSE:
			printf("MODBUS_TCP_MG_EV_CLOSE\n");
			break;
		case MG_EV_RECV:
			printf("MODBUS_TCP_MSG_RECEIVED\n");
			ModbusTCP_ParsePackage(io->buf,io->len,nc);
			mbuf_remove(io, io->len);       // Discard message from recv buffer
			break;
		default:
			break;
	}
}

void	*ModbusTCP_Task(void *arg)
{
  printf("<ModbusTCP Server> Task Created ! \n");
  while(1)
  {
    mg_mgr_poll(&modbusTcpServerMgr, 1000);
  }
  mg_mgr_free(&modbusTcpServerMgr);
}

void 	ModbusTCP_ParsePackage(uint8_t *data,uint16_t len,struct mg_connection *nc)
{
	uint16_t messageLength;
	uint8_t	 functionCode;
	uint16_t startRegisterAddress;
	if(len<6)
	{
		return; // minimum command size'dan dusuk..
	}
	messageLength = (data[4]<<8)|(data[5]);
	//check if all message received...
	if(len != messageLength+6)
	{
		return; // tum byte'lar alinamamis demektir..
	}
	functionCode 		 = data[7];
	startRegisterAddress = (data[8]<<8)|(data[9]);
	printf("Received Modbus TCP Package -> Function Code :%d , Start Address :%d\n",functionCode,startRegisterAddress);
	
	//burada bi yerde cevap verilecek, dolayisi ile client' a da ihtiyacimiz var..
	mg_send(nc, data, len);
	return;
}
	