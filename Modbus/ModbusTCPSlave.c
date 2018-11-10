#include "ModbusTCPSlave.h"


//Constants

const char *modbus_tcp_port = "503";

void 			Initialize_ModbusTCP( const char *portNumberParam )
{
	//printf("Initialize_ModbusTCP with port %s \n",portNumberParam );
	mg_mgr_init(&modbusTcpServerMgr, NULL);
	mg_bind(&modbusTcpServerMgr, portNumberParam, modbusTCP_EventHandler);
	pthread_create(&modbusTcpTask,NULL, ModbusTCP_Task,NULL);
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
			//printf("MODBUS_TCP_MSG_RECEIVED\n");
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
    mg_mgr_poll( &modbusTcpServerMgr, 100 );
  }
  mg_mgr_free( &modbusTcpServerMgr );
}


void  ModbusTCP_ParsePackage(uint8_t *data,uint16_t len,struct mg_connection *nc)
{
  uint16_t messageLength;
  uint16_t protocoolID;
  uint16_t transactionID;
  uint8_t  functionCode;
  uint8_t  unitId;
  uint16_t startRegisterAddress;
  uint16_t totalRegisterRequested;

  if(len<6)
  {
    return; // minimum command size'dan dusuk..
  }

  transactionID = (data[0]<<8)|(data[1]);
  protocoolID = (data[2]<<8)|(data[3]);
  if(protocoolID !=0)
  {
    return;
  }
  messageLength = (data[4]<<8)|(data[5]);
  //check if all message received...
  if(len != messageLength+6)
  {
    return; // tum byte'lar alinamamis demektir..
  }
  unitId    = data[6]; // bu ne aq modbus tcp de ne ip'si... gelen ip'yi output'a koyuyoruz direkt amac protokol yerini bulsun
  functionCode     = data[7];
  startRegisterAddress = (data[8]<<8)|(data[9]);
  totalRegisterRequested = (data[10]<<8)|(data[11]);
  uint16_t registerValues[totalRegisterRequested];
  //printf("Received Modbus TCP Package -> Function Code :%d , Start Address :%d Register Count :%d\n",functionCode,startRegisterAddress,totalRegisterRequested);
  switch(functionCode)
  {
    case READ_COILS:

      break;
    case READ_DISCRETE_INPUTS:

      break;
    case READ_HOLDING_REGISTERS:
      Modbus_ReadHoldingRegisters( startRegisterAddress,totalRegisterRequested, modbusTcpPDUBuffer+7,&modbusTcpPDULength);
      modbusTcpPDUBuffer[0] = (uint8_t)(transactionID>>8);
      modbusTcpPDUBuffer[1] = (uint8_t)(transactionID&0xff);
      modbusTcpPDUBuffer[2] = 0x00;
      modbusTcpPDUBuffer[3] = 0x00;
      modbusTcpPDUBuffer[4] = (uint8_t)((modbusTcpPDULength+1)>>8); // burada neden +1 artiyoruz cunki, bizim fonksiyonlarimiz pdu length donuyor, buradaki length pdu + unit id bytelarinin lengthi
      modbusTcpPDUBuffer[5] = (uint8_t)((modbusTcpPDULength+1)&0xff);
      modbusTcpPDUBuffer[6] = unitId;
      mg_send(nc,modbusTcpPDUBuffer,modbusTcpPDULength+7);
      break;
    case READ_INPUT_REGISTERS:
      Modbus_ReadInputRegisters( startRegisterAddress,totalRegisterRequested, modbusTcpPDUBuffer+7,&modbusTcpPDULength);
      modbusTcpPDUBuffer[0] = (uint8_t)(transactionID>>8);
      modbusTcpPDUBuffer[1] = (uint8_t)(transactionID&0xff);
      modbusTcpPDUBuffer[2] = 0x00;
      modbusTcpPDUBuffer[3] = 0x00;
      modbusTcpPDUBuffer[4] = (uint8_t)((modbusTcpPDULength+1)>>8); // burada neden +1 artiyoruz cunki, bizim fonksiyonlarimiz pdu length donuyor, buradaki length pdu + unit id bytelarinin lengthi
      modbusTcpPDUBuffer[5] = (uint8_t)((modbusTcpPDULength+1)&0xff);
      modbusTcpPDUBuffer[6] = unitId;
      mg_send(nc,modbusTcpPDUBuffer,(modbusTcpPDULength+7));
      break;
    case WRITE_SINGLE_COIL:
      break;
    case WRITE_SINGLE_HOLDING_REGISTER:
      registerValues[0] = (data[10]<<8)|(data[11]);
      Modbus_WriteSingleHoldingRegister(startRegisterAddress, registerValues[0],modbusTcpPDUBuffer+7,&modbusTcpPDULength);
      modbusTcpPDUBuffer[0] = (uint8_t)(transactionID>>8);
      modbusTcpPDUBuffer[1] = (uint8_t)(transactionID&0xff);
      modbusTcpPDUBuffer[2] = 0x00;
      modbusTcpPDUBuffer[3] = 0x00;
      modbusTcpPDUBuffer[4] = (uint8_t)((modbusTcpPDULength+1)>>8); // burada neden +1 artiyoruz cunki, bizim fonksiyonlarimiz pdu length donuyor, buradaki length pdu + unit id bytelarinin lengthi
      modbusTcpPDUBuffer[5] = (uint8_t)((modbusTcpPDULength+1)&0xff);
      modbusTcpPDUBuffer[6] = unitId;
      mg_send(nc,modbusTcpPDUBuffer,(modbusTcpPDULength+7));
      break;
    case WRITE_MULTIPLE_COILS:
      break;
    case WRITE_MULTIPLE_HOLDING_REGISTERS:
      break;
    default:
      //todo: send ILLEGAL_FUNCTION
      break;
  }
  return;
}
