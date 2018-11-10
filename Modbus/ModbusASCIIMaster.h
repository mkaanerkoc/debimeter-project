#ifndef MODBUS_ASCII_MASTER_H
#define MODBUS_ASCII_MASTER_H

#include "SerialPortDriver.h"
#include "ModbusMaster_Core.h"

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

extern ModbusMasterResult ModbusASCII_ReadInputRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam, uint8_t registerCount, uint16_t *registerValues );
extern ModbusMasterResult ModbusASCII_ReadHoldingRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam, uint8_t registerCount, uint16_t *registerValues );
extern ModbusMasterResult ModbusASCII_WriteHoldingRegister( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t valueParam );
extern ModbusMasterResult ModbusASCII_WriteHoldingRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t *valuesParam, uint8_t registerCount );


#endif // #ifndef MODBUS_ASCII_MASTER_H
