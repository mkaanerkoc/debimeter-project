#include "ModbusCore.h"





ModbusReturnType      	Modbus_ReadCoils(uint16_t startAddress, uint16_t registerCount, uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	uint16_t tempFrameLength=0;
	// response PDU : 	1 byte function code ,
	//					1 byte : number of data bytes as follow
	//					ceil( registerCount / 8 ) bytes : data bytes
	tempFrameLength = (int)(registerCount / 8);
	if(registerCount % 8 != 0)
	{
		tempFrameLength++;
	}
	tempFrameLength = tempFrameLength + 2;
	uint8_t tempFrame[tempFrameLength];
	tempFrame[0] = READ_COILS;
	tempFrame[1] = tempFrameLength-2;

	ModbusReturnType retVal;
	retVal = MODBUS_REGISTER_NOT_EXIST;

	return retVal;
}
ModbusReturnType     	Modbus_ReadDiscreteInputs(uint16_t startAddress, uint16_t registerCount, uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	uint16_t tempFrameLength=0;
	ModbusReturnType retVal;
	// response PDU : 	1 byte function code ,
	//					1 byte : number of data bytes as follow
	//					ceil( registerCount / 8 ) bytes : data bytes
	tempFrameLength = (int)(registerCount / 8);
	if(registerCount % 8 != 0)
	{
		tempFrameLength++;
	}
	tempFrameLength = tempFrameLength + 2;
	uint8_t tempFrame[tempFrameLength];
	tempFrame[0] = READ_DISCRETE_INPUTS;
	tempFrame[1] = tempFrameLength-2;


	retVal = MODBUS_REGISTER_NOT_EXIST;

	return retVal;
}

ModbusReturnType    Modbus_ReadHoldingRegisters(uint16_t startAddress, uint16_t registerCount, uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	ModbusReturnType retVal = MODBUS_SUCCESFUL;
	uint16_t tempFrameLength = 0;
	ModbusRegisterStruct *registerPtr;
	// response PDU : 	1 byte function code ,
	//					1 byte : number of data bytes as follow
	//					ceil( registerCount / 8 ) bytes : data bytes
	tempFrameLength = (int)(registerCount*2);
	tempFrameLength = tempFrameLength + 2;
	uint8_t tempFrame[tempFrameLength];
	tempFrame[0] = READ_HOLDING_REGISTERS;
	tempFrame[1] = tempFrameLength-2;
	for(int i = 0 ; i < registerCount ; i++)
	{
		registerPtr = Modbus_GetRegisterPointerByAddress(startAddress+i,HOLDING_REGISTER_TYPE);
		if(registerPtr!=NULL)
		{
			tempFrame[2+i*2] = (uint8_t)(registerPtr->value>>8);
			tempFrame[3+i*2] = (uint8_t)(registerPtr->value&0xFF);
			//printf("%d. Holding Register Value : %d\n",registerPtr->address,registerPtr->value);
		}
		else
		{
			retVal = MODBUS_REGISTER_NOT_EXIST;
			tempFrame[0] = tempFrame[0] + 0x80; //The response function code = the request function code + 0x80 ;
			tempFrame[1] = ILLEGAL_DATA_ADDRESS;
			tempFrameLength = 2;
			memcpy(outputPDUFrame,tempFrame,tempFrameLength);
			*outputPDULength = tempFrameLength;
			return retVal;
		}
	}

	if(retVal == MODBUS_SUCCESFUL)
	{
		memcpy(outputPDUFrame,tempFrame,tempFrameLength);
		*outputPDULength = tempFrameLength;
	}
	return retVal;
}

ModbusReturnType      	Modbus_ReadInputRegisters(uint16_t startAddress, uint16_t registerCount, uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	ModbusReturnType retVal = MODBUS_SUCCESFUL;
	uint16_t tempFrameLength=0;
	ModbusRegisterStruct *registerPtr;
	// response PDU : 	1 byte function code ,
	//					1 byte : number of data bytes as follow
	//					ceil( registerCount / 8 ) bytes : data bytes
	tempFrameLength = (int)(registerCount*2);
	tempFrameLength = tempFrameLength + 2;
	uint8_t tempFrame[tempFrameLength];
	tempFrame[0] = READ_INPUT_REGISTERS;
	tempFrame[1] = tempFrameLength-2;
	for(int i = 0 ; i < registerCount ; i++)
	{
		registerPtr = Modbus_GetRegisterPointerByAddress(startAddress+i,INPUT_REGISTER_TYPE);
		if(registerPtr!=NULL)
		{
			tempFrame[2+i*2] = (uint8_t)(registerPtr->value>>8);
			tempFrame[3+i*2] = (uint8_t)(registerPtr->value&0xFF);
		}
		else
		{
			retVal = MODBUS_REGISTER_NOT_EXIST;
			tempFrame[0] = tempFrame[0] + 0x80; //The response function code = the request function code + 0x80 ;
			tempFrame[1] = ILLEGAL_DATA_ADDRESS;
			tempFrameLength = 2;
			memcpy(outputPDUFrame,tempFrame,tempFrameLength);
			*outputPDULength = tempFrameLength;
			return retVal;
		}
	}
	if(retVal == MODBUS_SUCCESFUL)
	{
		memcpy(outputPDUFrame,tempFrame,tempFrameLength);
		*outputPDULength = tempFrameLength;
	}
	return retVal;
}

ModbusReturnType      	Modbus_WriteSingleCoil(uint16_t registerAddress, uint16_t setValue,uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	ModbusReturnType retVal;
	retVal = MODBUS_REGISTER_NOT_EXIST;
	return retVal;
}
ModbusReturnType		Modbus_WriteSingleHoldingRegister(uint16_t registerAddress,uint16_t setValue,uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	ModbusReturnType retVal = MODBUS_SUCCESFUL;
	uint16_t tempFrameLength=5;
	ModbusRegisterStruct *registerPtr;
	uint8_t tempFrame[tempFrameLength];
	tempFrame[0] = WRITE_SINGLE_HOLDING_REGISTER;
	registerPtr = Modbus_GetRegisterPointerByAddress(registerAddress,HOLDING_REGISTER_TYPE);
	if(registerPtr != NULL)
	{
		registerPtr->value = setValue;
		tempFrame[1] = (uint8_t)(registerAddress>>8);
		tempFrame[2] = (uint8_t)(registerAddress);
		tempFrame[3] = (uint8_t)(registerPtr->value>>8);
		tempFrame[4] = (uint8_t)(registerPtr->value&0xFF);
		memcpy(outputPDUFrame,tempFrame,tempFrameLength);
		*outputPDULength = tempFrameLength;
		//kaanbak -> burada callback fonksiyonu cagiracagiz..
	}
	else
	{
		tempFrame[0] = tempFrame[0] + 0x80; //The response function code = the request function code + 0x80 ;
		tempFrame[1] = ILLEGAL_DATA_ADDRESS;
		tempFrameLength = 2;
		memcpy(outputPDUFrame,tempFrame,tempFrameLength);
		*outputPDULength = tempFrameLength;
		retVal = MODBUS_REGISTER_NOT_EXIST;
	}
	return retVal;
}
ModbusReturnType		Modbus_WriteMultipleCoils(uint16_t startAddress,uint16_t registerCount, uint16_t *setValues,uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	ModbusReturnType retVal;
	retVal = MODBUS_REGISTER_NOT_EXIST;
	return retVal;
}
ModbusReturnType		Modbus_WriteMultipleHoldingRegisters(uint16_t startAddress,uint16_t registerCount,uint16_t *setValues,uint8_t *outputPDUFrame,uint16_t *outputPDULength)
{
	ModbusReturnType retVal;
	retVal = MODBUS_REGISTER_NOT_EXIST;
	return retVal;
}


ModbusReturnType Modbus_AddRegister(uint16_t registerAddress,ModbusRegisterType type)
{
	ModbusRegisterStruct *registerStr;
	ModbusReturnType retVal;
	retVal = MODBUS_ADD_FAILED_BUFFER_FULL;
	registerStr = Modbus_GetRegisterPointerByAddress(registerAddress,type);
	if(registerStr == NULL) // daha once olan bi register degil
	{
		if(COIL_TYPE == type)
		{
			if(coilRegisterCount >= MODBUS_COIL_REGISTER_COUNT)
			{
				retVal = MODBUS_ADD_FAILED_BUFFER_FULL;
			}
			else
			{
				registerStr =  &(modbusCoils[coilRegisterCount]);
				registerStr->address = registerAddress;
				registerStr->value = 0;
				registerStr->state = REGISTER_FULL;
				coilRegisterCount++;
				retVal = MODBUS_SUCCESFUL;
			}
		}
		else if(DISCRETE_INPUT_TYPE == type)
		{
			if(discreteInputCount >= MODBUS_DISCRETE_INPUT_REGISTER_COUNT)
			{
				retVal = MODBUS_ADD_FAILED_BUFFER_FULL;
			}
			else
			{
				registerStr = &(modbusDiscreteInputs[discreteInputCount]);
				registerStr->address = registerAddress;
				registerStr->value = 0;
				registerStr->state = REGISTER_FULL;
				discreteInputCount++;
				retVal = MODBUS_SUCCESFUL;
			}
		}
		else if(INPUT_REGISTER_TYPE == type)
		{
			if(inputRegisterCount >= MODBUS_INPUT_REGISTERS_COUNT)
			{
				retVal = MODBUS_ADD_FAILED_BUFFER_FULL;
			}
			else
			{
				registerStr = &(modbusInputRegisters[inputRegisterCount]);
				registerStr->address = registerAddress;
				registerStr->value = 0;
				registerStr->state = REGISTER_FULL;
				inputRegisterCount ++;
				retVal = MODBUS_SUCCESFUL;
			}
		}
		else if(HOLDING_REGISTER_TYPE == type)
		{
			if(holdingRegisterCount >= MODBUS_HOLDING_REGISTERS_COUNT)
			{
				retVal = MODBUS_ADD_FAILED_BUFFER_FULL;
			}
			else
			{
				registerStr = &(modbusHoldingRegisters[holdingRegisterCount]);
				registerStr->address = registerAddress;
				registerStr->value = 0;
				registerStr->state = REGISTER_FULL;
				holdingRegisterCount++;
				retVal = MODBUS_SUCCESFUL;
			}
		}
		else
		{
			retVal = MODBUS_INVALID_TYPE;
		}
	}
	else
	{
		retVal = MODBUS_REGISTER_EXIST_CANNOT_ADD;
	}
	return retVal;
}
ModbusReturnType Modbus_DeleteRegister(uint16_t registerAddress,ModbusRegisterType type)
{
	ModbusReturnType retVal;
	retVal = MODBUS_REGISTER_NOT_EXIST;
	return retVal;
}
ModbusReturnType Modbus_SetRegisterValue(uint16_t registerAddress,ModbusRegisterType type,uint16_t setValue)
{
	ModbusReturnType retVal;
	ModbusRegisterStruct *registerStr;
	registerStr = Modbus_GetRegisterPointerByAddress(registerAddress,type);
	if(registerStr == NULL)
	{
		retVal = MODBUS_REGISTER_NOT_EXIST;
	}
	else
	{
		if(registerStr->state == REGISTER_EMPTY)
		{
			retVal = MODBUS_REGISTER_NOT_EXIST;
		}
		else
		{
			registerStr->value = setValue;
			retVal = MODBUS_SUCCESFUL;
		}
	}
	return retVal;
}
ModbusReturnType Modbus_GetRegisterValue(uint16_t registerAddress,ModbusRegisterType type,uint16_t *getValue)
{
	ModbusReturnType retVal;
	retVal = MODBUS_REGISTER_NOT_EXIST;
	return retVal;
}


ModbusRegisterStruct* 	Modbus_GetRegisterPointerByAddress(uint16_t registerAddress,ModbusRegisterType type)
{
	if(COIL_TYPE == type)
	{
		for(int i = 0 ; i < MODBUS_COIL_REGISTER_COUNT ; i++)
		{
			if(modbusCoils[i].state != REGISTER_EMPTY)
			{
				if(modbusCoils[i].address == registerAddress)
				{
					return &(modbusCoils[i]);
				}
			}
		}
	}
	else if(DISCRETE_INPUT_TYPE == type)
	{
		for(int i = 0 ; i < MODBUS_DISCRETE_INPUT_REGISTER_COUNT ; i++)
		{
			if(modbusDiscreteInputs[i].state != REGISTER_EMPTY)
			{
				if(modbusDiscreteInputs[i].address == registerAddress)
				{
					return &(modbusDiscreteInputs[i]);
				}
			}
		}
	}
	else if(INPUT_REGISTER_TYPE == type)
	{
		for(int i = 0 ; i < MODBUS_INPUT_REGISTERS_COUNT ; i++)
		{
			if(modbusInputRegisters[i].state != REGISTER_EMPTY)
			{
				if(modbusInputRegisters[i].address == registerAddress)
				{
					return &(modbusInputRegisters[i]);
				}
			}
		}
	}
	else if(HOLDING_REGISTER_TYPE == type)
	{
		for(int i = 0 ; i < MODBUS_HOLDING_REGISTERS_COUNT ; i++)
		{
			if(modbusHoldingRegisters[i].state != REGISTER_EMPTY)
			{
				if(modbusHoldingRegisters[i].address == registerAddress)
				{
					return &(modbusHoldingRegisters[i]);
				}
			}
		}
	}
	else
	{
		return NULL;
	}
	// BURAYA KADAR GELEBILMISSE ZATEN BULAMAMISIZ DEMEKTIR
	return NULL;
}


ModbusRegisterStruct*	Modbus_GetRegisterPointerByIndex(uint16_t index,ModbusRegisterType type)
{
	if(COIL_TYPE == type)
	{
		return &(modbusCoils[index]);
	}
	else if(DISCRETE_INPUT_TYPE == type)
	{
		return &(modbusDiscreteInputs[index]);
	}
	else if(INPUT_REGISTER_TYPE == type)
	{
		return &(modbusInputRegisters[index]);
	}
	else if(HOLDING_REGISTER_TYPE == type)
	{
		return &(modbusHoldingRegisters[index]);
	}
	else
	{
		return NULL;
	}
}
