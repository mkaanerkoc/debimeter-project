#ifndef MODBUS_CORE_H
#define MODBUS_CORE_H

#include "ModbusCore_TypeDefsConstants.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"
#include "string.h"

//Defines
#define MODBUS_COIL_REGISTER_COUNT 				10
#define MODBUS_DISCRETE_INPUT_REGISTER_COUNT 	10
#define MODBUS_INPUT_REGISTERS_COUNT			128
#define MODBUS_HOLDING_REGISTERS_COUNT          128


// Register Arrays //
ModbusRegisterStruct modbusCoils[MODBUS_COIL_REGISTER_COUNT];
ModbusRegisterStruct modbusDiscreteInputs[MODBUS_DISCRETE_INPUT_REGISTER_COUNT];
ModbusRegisterStruct modbusInputRegisters[MODBUS_INPUT_REGISTERS_COUNT];
ModbusRegisterStruct modbusHoldingRegisters[MODBUS_HOLDING_REGISTERS_COUNT];

//Array Last Indexes // kaanbak: bunlara gerek var mi zaten hepsini arka arkaya yaziyoruz.. count yeterli gibi
uint16_t 	coilRegisterIndex;
uint16_t 	discreteInputIndex;
uint16_t 	inputRegisterIndex;
uint16_t 	holdingRegisterIndex;

//Array Register Counts //
uint16_t 	coilRegisterCount;
uint16_t 	discreteInputCount;
uint16_t 	inputRegisterCount;
uint16_t 	holdingRegisterCount;


// modbus functions
ModbusReturnType      	Modbus_ReadCoils(uint16_t startAddress, uint16_t registerCount,uint8_t *outputPDUFrame,uint16_t *outputPDULength);
ModbusReturnType     	Modbus_ReadDiscreteInputs(uint16_t startAddress, uint16_t registerCount,uint8_t *outputPDUFrame,uint16_t *outputPDULength);
ModbusReturnType      	Modbus_ReadHoldingRegisters(uint16_t startAddress, uint16_t registerCount, uint8_t *outputPDUFrame,uint16_t *outputPDULength);
ModbusReturnType      	Modbus_ReadInputRegisters(uint16_t startAddress, uint16_t registerCount,uint8_t *outputPDUFrame,uint16_t *outputPDULength);

ModbusReturnType      	Modbus_WriteSingleCoil(uint16_t registerAddress, uint16_t setValue,uint8_t *outputPDUFrame,uint16_t *outputPDULength);
ModbusReturnType		Modbus_WriteSingleHoldingRegister(uint16_t registerAddress,uint16_t setValue,uint8_t *outputPDUFrame,uint16_t *outputPDULength);
ModbusReturnType		Modbus_WriteMultipleCoils(uint16_t startAddress,uint16_t registerCount, uint16_t *setValues,uint8_t *outputPDUFrame,uint16_t *outputPDULength);
ModbusReturnType		Modbus_WriteMultipleHoldingRegisters(uint16_t startAddress,uint16_t registerCount,uint16_t *setValues,uint8_t *outputPDUFrame,uint16_t *outputPDULength);

// core functions
ModbusReturnType 		Modbus_AddRegister(uint16_t registerAddress,ModbusRegisterType type);
ModbusReturnType 		Modbus_DeleteRegister(uint16_t registerAddress,ModbusRegisterType type);
ModbusReturnType 		Modbus_SetRegisterValue(uint16_t registerAddress,ModbusRegisterType type,uint16_t setValue);
ModbusReturnType 		Modbus_GetRegisterValue(uint16_t registerAddress,ModbusRegisterType type,uint16_t *getValue);

ModbusRegisterStruct* 	Modbus_GetRegisterPointerByAddress(uint16_t registerAddress,ModbusRegisterType type);
ModbusRegisterStruct*	Modbus_GetRegisterPointerByIndex(uint16_t index,ModbusRegisterType type);


#endif