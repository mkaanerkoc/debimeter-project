#include "ConfigurationManager.h"

#define RASPBERRY_COMPILE

#ifndef RASPBERRY_COMPILE
	#define CONFIG_FILE_PATH "/home/mkaanerkoc/Desktop/debimeter-project/config_file.ini"
#else
	#define CONFIG_FILE_PATH "/home/pi/debimeter/config_file.ini"
#endif
FILE *configFile;

/* private functions */

int16_t RemoveSpaces(char* source);

ConfigurationResultEnum ConfigurationManager_Begin( void )
{
    int16_t readLineByteCount = 0;
    char    *readLineBuffer = NULL;
    char    configValueStr[32];
    char    *configStartPtr = NULL;
    int     readLineLength = 0;
    //printf("<ConfigurationManager> ConfigurationManager_Begin function-call \n");
    ConfigurationResultEnum retValueTemp = CONFIG_SUCCESS;

    // config struct'ini default value'lar ile ilklendirelim...
    appConfigSt.ina219bI2cAddress       = 45;
    appConfigSt.flowMeterModbusAddress  = 1;
    appConfigSt.devicesReadPeriod       = 60;
    appConfigSt.modbusMasterDataBit     = 8;
    appConfigSt.modbusMasterStopBit     = 1;
    appConfigSt.modbusMasterParity      = 0;

    appConfigSt.flowModbusRegisterAddress   = 100;
    appConfigSt.heightModbusRegisterAddress = 120;

    appConfigSt.useNotchFormula             = 0;
    appConfigSt.heightOffsetValue           = 0;
    appConfigSt.notchWidth                  = 5.0;
    
    strcpy( appConfigSt.modbusTcpPortNumber, "503" );
    strcpy( appConfigSt.serialPortName, "/dev/ttyUSB0" );


    configFile = fopen( CONFIG_FILE_PATH, "r" );
    if ( configFile == NULL )
    {
        retValueTemp = CONFIG_FILE_COULD_NOT_FOUND;
		printf("<ConfigurationManager> config file could not found, path : %s\n", CONFIG_FILE_PATH );
        return retValueTemp;
    }
    while( ( readLineByteCount = getline(&readLineBuffer, &readLineLength, configFile) ) != -1  )
    {
        readLineBuffer[ strcspn(readLineBuffer, "\n") ] = 0;
        readLineByteCount = RemoveSpaces( readLineBuffer );
        memset(configValueStr, 0 , sizeof(configValueStr));
        int dummyReadByte = 0;
        float dummyReadFloat = 0.0f;
        if( ( configStartPtr = strstr( readLineBuffer, MODBUS_MASTER_SERIAL_PORT_TAG ) ) != NULL )
        {
            strncpy( appConfigSt.serialPortName, configStartPtr + strlen( MODBUS_MASTER_SERIAL_PORT_TAG ) , ( readLineByteCount - strlen( MODBUS_MASTER_SERIAL_PORT_TAG ) ) );
        }
        else if( ( configStartPtr = strstr( readLineBuffer, MODBUS_MASTER_BAUD_RATE_TAG ) ) != NULL )
        {
            //printf("<ConfigurationManager> MODBUS_MASTER_BAUD_RATE_TAG found \n");

        }
        else if( ( configStartPtr = strstr( readLineBuffer, MODBUS_MASTER_PARITY_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( MODBUS_MASTER_PARITY_TAG ) , ( readLineByteCount - strlen( MODBUS_MASTER_PARITY_TAG ) ) );
            sscanf( configValueStr, "%d", &( appConfigSt.modbusMasterParity ));
        }
        else if( ( configStartPtr = strstr( readLineBuffer, MODBUS_MASTER_DATA_BIT_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( MODBUS_MASTER_DATA_BIT_TAG ) , ( readLineByteCount - strlen( MODBUS_MASTER_DATA_BIT_TAG ) ) );
            sscanf( configValueStr, "%d", &( appConfigSt.modbusMasterDataBit ));
        }
        else if( ( configStartPtr = strstr( readLineBuffer, MODBUS_MASTER_STOP_BIT_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( MODBUS_MASTER_STOP_BIT_TAG ) , ( readLineByteCount - strlen( MODBUS_MASTER_STOP_BIT_TAG ) ) );
            sscanf( configValueStr, "%d", &( appConfigSt.modbusMasterStopBit ));
        }
        else if( ( configStartPtr =  strstr( readLineBuffer, MODBUS_TCP_PORT_NUMBER_TAG ) ) != NULL )
        {
            strncpy( appConfigSt.modbusTcpPortNumber, configStartPtr + strlen( MODBUS_TCP_PORT_NUMBER_TAG ) , ( readLineByteCount - strlen( MODBUS_TCP_PORT_NUMBER_TAG ) ) );
            //sscanf( configValueStr, "%d", &( appConfigSt.modbusTcpPortNumber ));
        }
        else if( ( configStartPtr = strstr( readLineBuffer, READ_PERIOD_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( READ_PERIOD_TAG ) , ( readLineByteCount - strlen( READ_PERIOD_TAG ) ) );
            sscanf( configValueStr, "%d", &dummyReadByte);
            appConfigSt.devicesReadPeriod = dummyReadByte;
        }
        else if( ( configStartPtr = strstr( readLineBuffer, DATA_LOG_PERIOD_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( DATA_LOG_PERIOD_TAG ) , ( readLineByteCount - strlen( DATA_LOG_PERIOD_TAG ) ) );
            sscanf( configValueStr, "%d", &dummyReadByte);
            appConfigSt.dataLogPeriod = dummyReadByte;
        }
        else if( ( configStartPtr =  strstr( readLineBuffer, INA219B_I2C_ADDRESS_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( INA219B_I2C_ADDRESS_TAG ) , ( readLineByteCount - strlen( INA219B_I2C_ADDRESS_TAG ) ) );
            //sscanf( configValueStr, "%x", &(appConfigSt.ina219bI2cAddress));
        }
        else if( ( configStartPtr = strstr( readLineBuffer, FLOWMETER_MODBUS_ADDRESS_TAG ) ) != NULL ) 
        {
            strncpy( configValueStr, configStartPtr + strlen( FLOWMETER_MODBUS_ADDRESS_TAG ) , ( readLineByteCount - strlen( FLOWMETER_MODBUS_ADDRESS_TAG ) ) );

            sscanf( configValueStr, "%d", &dummyReadByte );
            appConfigSt.flowMeterModbusAddress = dummyReadByte;
        }
        else if( ( configStartPtr = strstr( readLineBuffer, FLOW_MODBUS_REGISTER_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( FLOW_MODBUS_REGISTER_TAG ) , ( readLineByteCount - strlen( FLOW_MODBUS_REGISTER_TAG ) ) );
            sscanf( configValueStr, "%d", &dummyReadByte );
            appConfigSt.flowModbusRegisterAddress = dummyReadByte;
        }
        else if( ( configStartPtr =  strstr( readLineBuffer, HEIGHT_MODBUS_REGISTER_TAG ) ) != NULL )
        {
            strncpy( configValueStr, configStartPtr + strlen( HEIGHT_MODBUS_REGISTER_TAG ) , ( readLineByteCount - strlen( HEIGHT_MODBUS_REGISTER_TAG ) ) );
            sscanf( configValueStr, "%d", &dummyReadByte );
            appConfigSt.heightModbusRegisterAddress = dummyReadByte;
        }
        else if( ( configStartPtr = strstr( readLineBuffer, HEIGHT_OFFSET_VALUE_TAG ) ) != NULL ) 
        {
            strncpy( configValueStr, configStartPtr + strlen( HEIGHT_OFFSET_VALUE_TAG ) , ( readLineByteCount - strlen( HEIGHT_OFFSET_VALUE_TAG ) ) );

            sscanf( configValueStr, "%d", &dummyReadByte );
            appConfigSt.heightOffsetValue = dummyReadByte;
        }
        else if( ( configStartPtr = strstr( readLineBuffer, ADS1115_RESISTOR_VALUE_TAG ) ) != NULL ) 
        {
            strncpy( configValueStr, configStartPtr + strlen( ADS1115_RESISTOR_VALUE_TAG ) , ( readLineByteCount - strlen( ADS1115_RESISTOR_VALUE_TAG ) ) );

            sscanf( configValueStr, "%f", &dummyReadFloat );
            appConfigSt.ads1115ResistorValue = dummyReadFloat;
        }
        else if( ( configStartPtr = strstr( readLineBuffer, ADS1115_4MA_HEIGHT_VALUE_TAG ) ) != NULL ) 
        {
            strncpy( configValueStr, configStartPtr + strlen( ADS1115_4MA_HEIGHT_VALUE_TAG ) , ( readLineByteCount - strlen( ADS1115_4MA_HEIGHT_VALUE_TAG ) ) );

            sscanf( configValueStr, "%f", &dummyReadFloat );
            appConfigSt.ads1115_4mA_Value = dummyReadFloat;
        }
        else if( ( configStartPtr = strstr( readLineBuffer, ADS1115_20MA_HEIGHT_VALUE_TAG ) ) != NULL ) 
        {
            strncpy( configValueStr, configStartPtr + strlen( ADS1115_20MA_HEIGHT_VALUE_TAG ) , ( readLineByteCount - strlen( ADS1115_20MA_HEIGHT_VALUE_TAG ) ) );

            sscanf( configValueStr, "%f", &dummyReadFloat );
            appConfigSt.ads1115_20mA_Value = dummyReadFloat;
        }
        else if( ( configStartPtr = strstr( readLineBuffer, USE_NOTCH_FORMULA_TAG ) ) != NULL ) 
        {
            strncpy( configValueStr, configStartPtr + strlen( USE_NOTCH_FORMULA_TAG ) , ( readLineByteCount - strlen( USE_NOTCH_FORMULA_TAG ) ) );

            sscanf( configValueStr, "%d", &dummyReadByte );
            if( ( 0 == dummyReadByte ) || ( 1 == dummyReadByte ) )
            {
                appConfigSt.useNotchFormula = dummyReadByte;
            }
            else
            {
                appConfigSt.useNotchFormula = 0; // invalid giris yapilmis...
            }   
        }
        else if( ( configStartPtr = strstr( readLineBuffer, NOTCH_WIDTH_TAG ) ) != NULL ) 
        {
            strncpy( configValueStr, configStartPtr + strlen( NOTCH_WIDTH_TAG ) , ( readLineByteCount - strlen( NOTCH_WIDTH_TAG ) ) );

            sscanf( configValueStr, "%f", &dummyReadFloat );
            if( ( dummyReadFloat >= 10 ) && ( dummyReadFloat <= 25) )
            {
                appConfigSt.notchWidth = dummyReadFloat;
            }
            else
            {
                appConfigSt.notchWidth = 15; // invalid giris yapilmis...
            }
            
        }
    }

    fclose( configFile );
    if( readLineBuffer )
    {
        free( readLineBuffer );
    }
    return CONFIG_SUCCESS;
}



ConfigurationResultEnum ConfigurationManager_GetValue( const char *keyParam, char *valueParam )
{
    int16_t readLineByteCount = 0;
    char    *readLineBuffer = NULL;
    char    *configStartPtr = NULL;
    int     readLineLength = 0;
    ConfigurationResultEnum retValueTemp = CONFIG_SUCCESS;
    configFile = fopen( CONFIG_FILE_PATH, "r" );
    if ( configFile == NULL )
    {
        retValueTemp = CONFIG_FILE_COULD_NOT_FOUND;
        return retValueTemp;
    }
    while( ( readLineByteCount = getline(&readLineBuffer, &readLineLength, configFile) ) != -1  )
    {
        readLineByteCount = RemoveSpaces( readLineBuffer );
        if( ( configStartPtr = strstr( readLineBuffer, keyParam ) ) != NULL )
        {
            strncpy( valueParam, configStartPtr + strlen( keyParam ) , ( readLineByteCount - strlen( keyParam ) ) );
        }
    }
    fclose ( configFile );
    return retValueTemp;
}

ConfigurationResultEnum ConfigurationManager_SetValue( const char *keyParam, char *valueParam )
{
    int16_t readLineByteCount = 0;
    char    *readLineBuffer = NULL;
    char    *configStartPtr = NULL;
    int     readLineLength = 0;
    ConfigurationResultEnum retValueTemp = CONFIG_SUCCESS;
    configFile = fopen( CONFIG_FILE_PATH, "w" );
    if ( configFile == NULL )
    {
        retValueTemp = CONFIG_FILE_COULD_NOT_FOUND;
        return retValueTemp;
    }
    while( ( readLineByteCount = getline(&readLineBuffer, &readLineLength, configFile) ) != -1  )
    {
    }
    fclose ( configFile );
    return retValueTemp;
}

/* private functions */

int16_t RemoveSpaces(char* source)
{
    int16_t retValueTemp = 0;
    char* i = source;
    char* j = source;
    while(*j != 0)
    {
        *i = *j++;
        if(*i != ' ')
        {
            i++;
            retValueTemp++;
        }
    }
    *i = 0;
    return retValueTemp;
}

void ConfigurationManager_PrintConfiguration( ApplicationConfiguration *config )
{
	printf("*** ApplicationConfiguration Values Begin ***\n");
	printf("Serial Port Name : %s \n", config->serialPortName );
	printf("Devices Read Period : %d \n", config->devicesReadPeriod );
    printf("Data Logging Period : %d \n", config->dataLogPeriod );
	printf("Modbus TCP Port Number : %s \n", config->modbusTcpPortNumber );
	printf("Flow ModbusTCP Register Address : %d \n", config->flowModbusRegisterAddress );
	printf("Height ModbusTCP Register Address : %d \n", config->heightModbusRegisterAddress );
    printf("Height offset : %d \n", config->heightOffsetValue );
	printf("Phoenix FlowMeter ModbusASCII Slave ID : %d \n", config->flowMeterModbusAddress);
	printf("ADS1115 Resistor Value : %f \n", config->ads1115ResistorValue );
	printf("ADS1115 4mA Height Value : %f \n", config->ads1115_4mA_Value );
	printf("ADS1115 20mA Height Value : %f \n", config->ads1115_20mA_Value );
    printf("Use Notch Formula Value : %d \n", config->useNotchFormula );
    printf("Notch Width Value : %.3f \n", config->notchWidth );
	printf("*** ApplicationConfiguration Values End ***\n");
}