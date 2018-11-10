#ifndef MODBUS_CORE_TYPE_DEFS_CONSTANTS_H
#define MODBUS_CORE_TYPE_DEFS_CONSTANTS_H

#include "stdint.h"


// function codes
typedef enum ModbusFunctionCodes
{
	READ_COILS = 1,
	READ_DISCRETE_INPUTS = 2,
	READ_HOLDING_REGISTERS = 3,
	READ_INPUT_REGISTERS = 4,
	WRITE_SINGLE_COIL	= 5,
	WRITE_SINGLE_HOLDING_REGISTER = 6,
	WRITE_MULTIPLE_COILS = 15,
	WRITE_MULTIPLE_HOLDING_REGISTERS = 16
}ModbusFunctionCodes;

typedef enum ModbusExceptionCodes
{
	ILLEGAL_FUNCTION = 1, // 	The function code received in the query is not recognized by the slave or is not allowed by the slave.
	ILLEGAL_DATA_ADDRESS = 2, // The data address (register number) received in the query is not an allowed address for the slave,
							//i.e., the register does not exist. If multiple registers were requested, at least one was not permitted.
	ILLEGAL_DATA_VALUE = 3, // 	The value contained in the query's data field is not acceptable to the slave.
	SLAVE_DEVICE_FAILURE = 4, // An unrecoverable error occurred while the slave was attempting to perform the requested action
	SLAVE_DEVICE_BUSY = 6, // The slave is engaged in processing a long-duration command. The master should try again later.
	GATEWAY_PATH_UNAVAILABLE = 10, // Specialized use in conjunction with gateways, usually means the gateway is misconfigured or overloaded
	GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 11 // Specialized use in conjunction with gateways, indicates no response was received
												// from the target device.
}ModbusExceptionCodes;

typedef enum ModbusRegisterTableOffsets
{
	COIL_OFFSET 			= 00001,
	DISCRETE_INPUT_OFFSET 	= 10001,
	INPUT_REGISTER_OFFSET 	= 30001,
	HOLDING_REGISTER_OFFSET = 40001
}ModbusRegisterTableOffsets;

typedef enum ModbusRegisterOpType
{
	READ_WRITE = 1,
	READ_ONLY = 2
}ModbusRegisterOpType;

typedef enum ModbusRegisterType
{
	COIL_TYPE,
	DISCRETE_INPUT_TYPE,
	INPUT_REGISTER_TYPE,
	HOLDING_REGISTER_TYPE
}ModbusRegisterType;

typedef enum ModbusReturnType
{
	MODBUS_SUCCESFUL,
	MODBUS_OPERATION_NOT_ALLOWED,
	MODBUS_REGISTER_CANNOT_FOUND,
	MODBUS_ADD_FAILED_BUFFER_FULL,
	MODBUS_REGISTER_NOT_EXIST,
	MODBUS_REGISTER_EXIST_CANNOT_ADD,
	MODBUS_INVALID_TYPE
}ModbusReturnType;

typedef enum ModbusRegisterState
{
	REGISTER_EMPTY,
	REGISTER_FULL
}ModbusRegisterState;

typedef enum ModbusRegisterDataType
{
	UNSIGNED16_REGISTER,
	UNSIGNED32_REGISTER,
	SIGNED16_REGISTER,
	SIGNED32_REGISTER,
	FLOAT_REGISTER,
	FLOAT_REVERSE_REGISTER
}ModbusRegisterDataType;

typedef struct ModbusRegisterStruct
{
	ModbusRegisterType type;
	uint16_t address;
	uint16_t value;
	ModbusRegisterState state;
}ModbusRegisterStruct;

#endif
