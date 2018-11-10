#ifndef MODBUS_RTU_MASTER_H
#define MODBUS_RTU_MASTER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ModbusMaster_Core.h"
#include "SerialPortDriver.h"

extern ModbusMasterResult ModbusRTU_ReadInputRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam , uint16_t registerCount, uint16_t *registerValues );
extern ModbusMasterResult ModbusRTU_ReadHoldingRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam , uint16_t registerCount, uint16_t *registerValues );
extern ModbusMasterResult ModbusRTU_WriteHoldingRegister( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t valueParam );

#endif // end of #ifndef MODBUS_RTU_MASTER_H
