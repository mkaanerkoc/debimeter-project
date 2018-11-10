#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


// Config keys

#define     MODBUS_MASTER_SERIAL_PORT_TAG       "#MODBUS_MASTER_SERIAL_PORT:"
#define     MODBUS_MASTER_BAUD_RATE_TAG         "#MODBUS_MASTER_BAUD_RATE:"
#define     MODBUS_MASTER_PARITY_TAG            "#MODBUS_MASTER_PARITY:"
#define     MODBUS_MASTER_DATA_BIT_TAG          "#MODBUS_MASTER_DATA_BIT:"
#define     MODBUS_MASTER_STOP_BIT_TAG          "#MODBUS_MASTER_STOP_BIT:"
#define     MODBUS_TCP_PORT_NUMBER_TAG          "#MODBUS_TCP_PORT_NUMBER:"
#define     READ_PERIOD_TAG                     "#READ_PERIOD:"
#define 	DATA_LOG_PERIOD_TAG				    "#DATA_LOG_PERIOD:"
#define     INA219B_I2C_ADDRESS_TAG             "#INA219B_I2C_ADDRESS:"
#define     FLOWMETER_MODBUS_ADDRESS_TAG        "#FLOWMETER_MODBUS_ADDRESS:"
#define     FLOW_MODBUS_REGISTER_TAG            "#FLOW_MODBUS_REGISTER:"
#define     HEIGHT_MODBUS_REGISTER_TAG          "#HEIGHT_MODBUS_REGISTER:"
#define     HEIGHT_OFFSET_VALUE_TAG             "#HEIGHT_OFFSET_VALUE:"
#define 	ADS1115_RESISTOR_VALUE_TAG		    "#ADS1115_RESISTOR_VALUE:"
#define 	ADS1115_4MA_HEIGHT_VALUE_TAG	    "#ADS1115_4MA_HEIGHT_VALUE:"
#define		ADS1115_20MA_HEIGHT_VALUE_TAG       "#ADS1115_20MA_HEIGHT_VALUE:"
#define     USE_NOTCH_FORMULA_TAG               "#USE_NOTCH_FORMULA:"
#define     NOTCH_WIDTH_TAG                     "#NOTCH_WIDTH_VALUE:"

// config default values..

typedef enum ConfigurationResultEnum
{
    CONFIG_FAILED,
    CONFIG_FILE_COULD_NOT_FOUND,
    CONFIG_SUCCESS
}ConfigurationResultEnum;


typedef struct ApplicationConfiguration
{
    char        serialPortName[ 40 ];
    long long   modbusMasterBaudRate;
    int         modbusMasterParity;
    int         modbusMasterDataBit;
    int         modbusMasterStopBit;
    char        modbusTcpPortNumber[8];
    uint32_t   	devicesReadPeriod;
    uint32_t    dataLogPeriod;
    uint8_t     ina219bI2cAddress;
    uint16_t    flowMeterModbusAddress;
    uint16_t    flowModbusRegisterAddress;
    uint16_t    heightModbusRegisterAddress;
    uint16_t    heightOffsetValue;
    float       ads1115ResistorValue;
    float 		ads1115_4mA_Value;
    float 		ads1115_20mA_Value;
    int         useNotchFormula;
    float       notchWidth;
    char        ipAddress[32];
}ApplicationConfiguration;


ApplicationConfiguration appConfigSt;

extern ConfigurationResultEnum ConfigurationManager_Begin( void );
extern ConfigurationResultEnum ConfigurationManager_GetValue( const char *keyParam, char *valueParam );
extern ConfigurationResultEnum ConfigurationManager_SetValue( const char *keyParam, char *valueParam );

extern void ConfigurationManager_PrintConfiguration( ApplicationConfiguration *config );
#endif
