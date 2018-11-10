#ifndef MODBUS_MASTER_CORE_H
#define MODBUS_MASTER_CORE_H

#include <stdint.h>
#include <string.h>
#include "ModbusCore_TypeDefsConstants.h"

typedef struct ModbusSlaveDeviceRegister
{
    char                    allias[50];
    uint16_t                startAddress;
    ModbusRegisterType      type;
    ModbusRegisterDataType  dataType;
}ModbusSlaveDeviceRegister;

typedef enum ModbusFunctionCodesEnum
{
    READ_HOLDING_REGISTERS_FCODE = 0x03,
    READ_INPUT_REGISTERS_FCODE = 0x04,
    WRITE_HOLDING_REGISTER_FCODE = 0x06,
    WRITE_MULTI_HOLDING_REGISTERS_FCODE = 0x10
}ModbusFunctionCodesEnum;

typedef enum ModbusMasterResult
{
    MODBUS_READ_SUCCESS = 0,
    MODBUS_WRITE_SUCCESS = 1,
    MODBUS_INVALID_REGISTER_ADDRESS = 2,
    MODBUS_ERROR_ON_CREATING_MSG = 3,
    MODBUS_ERROR_ON_PARSING_RESP = 4,
    MODBUS_FRAME_CHECK_ERROR = 5,
    MODBUS_WRONG_ID_ERROR = 6,
    MODBUS_WRONG_FCODE_ERROR = 7,
    MODBUS_RESPONSE_TIMEOUT = 8
}ModbusMasterResult;

extern void ModbusMaster_PrepareWriteHoldingRegisterPDU( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t valueParam , uint8_t *pduParam, uint16_t *pduLengthParam  );
extern void ModbusMaster_PrepareWriteMultiHoldingRegistersPDU( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t *valuesArrayParam , uint8_t registerCountParam , uint8_t *pduParam , uint16_t *pduLengthParam );

extern void ModbusMaster_PrepareReadHoldingRegistersPDU( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t registerCountParam , uint8_t *pduParam , uint16_t *pduLengthParam  );
extern void ModbusMaster_PrepareReadInputRegistersPDU( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t registerCountParam , uint8_t *pduParam , uint16_t *pduLengthParam  );


#endif // end of #ifndef MODBUS_MASTER_CORE_H
