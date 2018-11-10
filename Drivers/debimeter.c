#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#include "SerialPortDriver.h"
#include <pthread.h>
#include "CommonTypeDefs.h"
#include "ModbusASCIIMaster.h"
#include "ModbusRTUMaster.h"
#include "ModbusTCPSlave.h"
#include "INA219BDriver.h"
#include "ADS1115Driver.h"
#include "RemoteControlManager.h"
#include "RemoteControl_Proxy.h"
#include "ConfigurationManager.h"
#include "RTCManager.h"
#include "IPManager.h"
#include "RavenEyenDriver.h"
#include "debimeter.h"
#include <math.h>

#define  FILE_LOG_HEADER    "Saat; V_QP; V_QPF; V_AVG; V_RAW; Temperature; Humidity; Pressure; Amplitude; Level; FlowRate; Milivolt; Miliampere; Height ( mm )"
#define  FILE_LOG_HEADER_SIZE strlen( FILE_LOG_HEADER )


uint16_t                    modbusAsciiRegisterArrayTemp[ 32 ];
uint16_t 					el200RegisterArrayTemp[32];
pthread_t 		            deviceMainTaskHandler;
void 			            *DeviceMainTask(void *arg);
void                        BeginMainTask(void);

ConfigurationResultEnum 	configManagerResultEnum;
ModbusMasterResult 			modbusResultTemp = MODBUS_WRONG_FCODE_ERROR;
uint8_t 					ina219ResultTemp = 0x01, ads1115ResultTemp = 0x01;
SerialPortResultEnum		serialPortResultTemp = PORT_FAILED;

RavenEyenRegisterStruct     ravenEyeInfo;
INA219BInfoStruct           ina219Info;
ADS1115InfoStruct           ads1115Info;
float                       dataBaseRowValues[ 64 ];
uint8_t                     dataBaseRowIndex = 0;
char                        ipAddress[32];
uint16_t                    totalWrittenRegisterCount = 0;

float                       convertToFloat( uint8_t *valuesParam, bool inversedParam );
uint32_t                    convertToLong( float floatValue, bool inversedParam );
void                        writeChannelValuesToModbusRegisters( WebScreen_DataPackage *dataParam );
void                        writeChannelValuesToDatabase( float *dbBuffer, uint8_t count );

ModbusMasterResult          Read_RavenEye_Values( RavenEyenRegisterStruct *ravenEyeInfo );
void                        Send_RavenEye_ValuesToRemoteController( RavenEyenRegisterStruct *ravenEyeInfo );
void                        Set_RavenEye_ValuesOnModbusTCP( RavenEyenRegisterStruct *ravenEyeInfo, uint16_t *writtenRegisterCount );
void                        Get_RavenEye_ValuesToDatabaseBuffer( RavenEyenRegisterStruct *ads1115Info, float *dbBuffer, uint8_t *count );

uint8_t                     Read_INA219B_Values( INA219BInfoStruct *ina219Info );
void                        Print_INA219B_Values( INA219BInfoStruct *ina219Info );
        
uint8_t                     Read_ADS1115_Values( ADS1115InfoStruct *ads1115Info );
void                        Print_ADS1115_Value( ADS1115InfoStruct *ads1115Info );
void                        Set_ADS1115_ValuesOnModbusTCP( ADS1115InfoStruct *ads1115Info, uint16_t *writtenRegisterCount );
void                        Send_ADS1115_ValuesToRemoteController( ADS1115InfoStruct *ads1115Info );
void                        Get_ADS1115_ValuesToDatabaseBuffer( ADS1115InfoStruct *ads1115Info, float *dbBuffer, uint8_t *count );
uint16_t                    Calculate_ADS1115_Values( int16_t adcDiffRead, float resistorValue, float _4mAHeightValue, float _20mAHeightValue, ADS1115InfoStruct *adsInfo );
float                       Calculate_FlowRateWithNotchFormula( uint16_t heightParam, float notchWidthParam );

void 						Set_EL200_ValuesOnModbusTCP( uint16_t *registerValues, uint16_t *writtenRegisterCount );

typedef union ModbusRegisterUnion
{
    float           fvalue;
    uint16_t        uvalue;
    unsigned char   bytes[4];
    uint16_t        words[2];
} ModbusRegisterUnion;

int main( void )
{
    printf("Debimeter viewer v.1.0.0 \n");

	configManagerResultEnum = ConfigurationManager_Begin();
    if( CONFIG_SUCCESS != configManagerResultEnum )
    {
        printf( "<Main> ConfigurationManager could not begin succesfully, default values loaded  %d \n", configManagerResultEnum );
    }
    else
    {
    	printf( "<Main> ConfigurationManager begun succesfully\n" );
    }
    /*ina219ResultTemp = INA219B_Begin( NON_THREAD, appConfigSt.ina219bI2cAddress );
    if( 0x00 != ina219ResultTemp )
    {
    	printf( "<Main> INA219B Could not begin succesfully, Reason : %d \n", ina219ResultTemp );
    }
    else
    {
    	printf( "<Main> INA219B begun succesfully \n" );
    }*/
    //ConfigurationManager_PrintConfiguration( &appConfigSt );
    ads1115ResultTemp = ADS1115_Begin( );
    if( 0x00 != ads1115ResultTemp )
    {
        printf( "<Main> ADS1115 Could not begin succesfully, Reason : %d \n", ads1115ResultTemp );
    }
    else
    {
        printf( "<Main> ADS1115 begun succesfully \n" );
    }

    ADS1115_SetGain( GAIN_ONE );

    serialPortResultTemp = SerialPort_Begin( appConfigSt.serialPortName, (speed_t)B19200, 8, 0, 2 );
    if( PORT_SUCCESS != serialPortResultTemp )
    {
    	printf( "<Main> SerialPort could not begin succesfully, Reason : %d \n", serialPortResultTemp );
    }
    else
    {
    	printf( "<Main> SerialPort begun succesfully \n" );
    }

    IPManager_GetIpAddress( appConfigSt.ipAddress );
    printf("<Application> IP Address -> %s \n", appConfigSt.ipAddress );
    Initialize_ModbusTCP( ( const char* ) appConfigSt.modbusTcpPortNumber );
    // Add ModbusRegisters //
    for( int k = 10 ; k < 130 ; k++ )
    {
    	Modbus_AddRegister( k, INPUT_REGISTER_TYPE );
    }
    
    RemoteControl_Initialize();
   
   

   	modbusResultTemp = Read_RavenEye_Values( &ravenEyeInfo );
    if( modbusResultTemp == MODBUS_READ_SUCCESS )
    {
        Print_RavenEye_Values( &ravenEyeInfo );
    }

    BeginMainTask();
    RemoteControl_Spawn(); 

    return 0;
}


void BeginMainTask(void)
{
	pthread_create( &deviceMainTaskHandler, NULL, DeviceMainTask, NULL );
}

void *DeviceMainTask(void *arg)
{
    TimeStruct_t        timerStTemp;
    DateStruct_t        dateStTemp;
    uint16_t            modbusValues[2];
    float               ina219BCurrentTemp;
    uint16_t            heightInformationTemp;
    float               current_mA = 0;
    float               loadvoltage = 0;
    float               power_mW = 0;
    int16_t             adc0, adc1, adcDifference;
    bool                deviceReadPeriodOccured = false;
    bool                dataLogPeriodOccured = false;
    uint8_t             deviceReadPeriodIndex = appConfigSt.devicesReadPeriod;
    uint8_t             dataLogPeriodIndex = appConfigSt.dataLogPeriod;
    while( 1 )
    {
        usleep( 1000000 ); // wait for 1 second 
        deviceReadPeriodIndex = deviceReadPeriodIndex - 1;
        dataLogPeriodIndex = dataLogPeriodIndex - 1;
        if( deviceReadPeriodIndex <= 0 )
        {
            deviceReadPeriodOccured = true;
        }
        if( dataLogPeriodIndex <= 0 )
        {
            dataLogPeriodOccured = true;
        }
        if( true == deviceReadPeriodOccured )
        {
            printf( "Device Read Period Occured \n");
            memset( dataBaseRowValues, 0 , sizeof( dataBaseRowValues ));
            memset( &( ads1115Info), 0, sizeof( ADS1115InfoStruct ));
            memset( &( ravenEyeInfo), 0, sizeof( RavenEyenRegisterStruct ));
            dataBaseRowIndex = 0;

            // read ADS1115 and calculate height in milimeters
            adc0 = ADS1115_readADC_SingleEnded(0);
            adc1 = ADS1115_readADC_SingleEnded(1);
            adcDifference = adc0 - adc1;
            Calculate_ADS1115_Values(  adcDifference, appConfigSt.ads1115ResistorValue, appConfigSt.ads1115_4mA_Value, appConfigSt.ads1115_20mA_Value, &ads1115Info );
            
            
            if( 0 == appConfigSt.useNotchFormula )
            {
            	SerialPort_Close();
            	serialPortResultTemp = SerialPort_Begin( appConfigSt.serialPortName, (speed_t)B19200, 8, 0, 2 );

                // write milimeter info to phoenix
                modbusResultTemp = ModbusASCII_WriteHoldingRegisters( appConfigSt.flowMeterModbusAddress, 4000, &( ads1115Info.height ), 1 );

                if( modbusResultTemp != MODBUS_WRITE_SUCCESS )
                {
                    printf("<Application> Writing to Raven-Eye is not succesfull... Check the connections ! \n");
                }
                else
                {
                    usleep( 2 * 1000000 ); // writing is succesfull, wait until the operation finished
                }
                
                modbusResultTemp = Read_RavenEye_Values( &ravenEyeInfo ); // read raven eye values
                uint16_t writtenHeightValueTemp[2] = { 0, 0 };
                uint16_t heightValueTemp = 0;
                ModbusASCII_ReadHoldingRegisters( appConfigSt.flowMeterModbusAddress, 4000, 1, &heightValueTemp );

                if( modbusResultTemp == MODBUS_READ_SUCCESS )
                {
                    Send_ADS1115_ValuesToRemoteController( &ads1115Info );
                    Send_RavenEye_ValuesToRemoteController( &ravenEyeInfo );
                    
                    Set_RavenEye_ValuesOnModbusTCP( &ravenEyeInfo, &totalWrittenRegisterCount );
                    Set_ADS1115_ValuesOnModbusTCP( &ads1115Info, &totalWrittenRegisterCount );

                    Get_RavenEye_ValuesToDatabaseBuffer( &ravenEyeInfo, dataBaseRowValues, &dataBaseRowIndex );
                    Get_ADS1115_ValuesToDatabaseBuffer( &ads1115Info, dataBaseRowValues, &dataBaseRowIndex );
                    if( true == dataLogPeriodOccured )
                    {
                        writeChannelValuesToDatabase( dataBaseRowValues, dataBaseRowIndex );
                        dataLogPeriodOccured = false;
                        dataLogPeriodIndex = appConfigSt.dataLogPeriod;
                    }
                }
            } // end of  if( appConfigSt.useNotchValue == 0 )
            else if ( 1 == appConfigSt.useNotchFormula )
            {
                float flowRate_Temp = Calculate_FlowRateWithNotchFormula( ads1115Info.height, appConfigSt.notchWidth );
                ravenEyeInfo.flowRate_Q = flowRate_Temp;
                Send_ADS1115_ValuesToRemoteController( &ads1115Info );
                Send_RavenEye_ValuesToRemoteController( &ravenEyeInfo );
                
                Set_RavenEye_ValuesOnModbusTCP( &ravenEyeInfo, &totalWrittenRegisterCount );
                Set_ADS1115_ValuesOnModbusTCP( &ads1115Info, &totalWrittenRegisterCount );

                Get_RavenEye_ValuesToDatabaseBuffer( &ravenEyeInfo, dataBaseRowValues, &dataBaseRowIndex );
                Get_ADS1115_ValuesToDatabaseBuffer( &ads1115Info, dataBaseRowValues, &dataBaseRowIndex );
                if( true == dataLogPeriodOccured )
                {
                    writeChannelValuesToDatabase( dataBaseRowValues, dataBaseRowIndex );
                    dataLogPeriodOccured = false;
                    dataLogPeriodIndex = appConfigSt.dataLogPeriod;
                }
            }
            // read el200
            /*memset( el200RegisterArrayTemp, 0, sizeof( el200RegisterArrayTemp ));
            SerialPort_Close();
            serialPortResultTemp = SerialPort_Begin( appConfigSt.serialPortName, (speed_t)B9600, 8, 0, 1 );
		    if( PORT_SUCCESS == serialPortResultTemp )
		    {
		    	modbusResultTemp = ModbusRTU_ReadHoldingRegisters( 5, 132, 32, el200RegisterArrayTemp );
		    }
		    else
		    {
		    	printf("error on port begin ... \n");
		    }
		   
		    if( modbusResultTemp == MODBUS_READ_SUCCESS )
		    {
		    	printf("EL200 read Success\n" );
		    	Set_EL200_ValuesOnModbusTCP( el200RegisterArrayTemp, &totalWrittenRegisterCount );
		    }
		    else
		    {
		    	printf("EL200 Read Fail :%d \n", modbusResultTemp );
		    }
		    // reset serial port config*/


            deviceReadPeriodOccured = false;
            deviceReadPeriodIndex = appConfigSt.devicesReadPeriod;
        }
    }
}

ModbusMasterResult  Read_RavenEye_Values( RavenEyenRegisterStruct *ravenEyeInfo )
{
    ModbusMasterResult      modbusResultTemp = MODBUS_WRONG_FCODE_ERROR;
    Initialize_RavenEye_Values( ravenEyeInfo );
    modbusResultTemp        = ModbusASCII_ReadHoldingRegisters( appConfigSt.flowMeterModbusAddress, 1000, 29, modbusAsciiRegisterArrayTemp );
    if( modbusResultTemp == MODBUS_READ_SUCCESS )
    {
        ravenEyeInfo->velocity_vQP      = convertToFloat( modbusAsciiRegisterArrayTemp, true);
        ravenEyeInfo->velocity_vQPF     = convertToFloat( modbusAsciiRegisterArrayTemp + 2, true );
        ravenEyeInfo->velocity_vAVG     = convertToFloat( modbusAsciiRegisterArrayTemp + 4, true );
        ravenEyeInfo->velocity_vRAW     = convertToFloat( modbusAsciiRegisterArrayTemp + 6, true );
        
        ravenEyeInfo->vsn               = ( int ) (*( modbusAsciiRegisterArrayTemp + 9 ));
        ravenEyeInfo->snr               = convertToFloat( modbusAsciiRegisterArrayTemp + 10, true );
        ravenEyeInfo->agc               = ( int ) (*( modbusAsciiRegisterArrayTemp + 12 ));
        ravenEyeInfo->NOT               = ( int ) (*( modbusAsciiRegisterArrayTemp + 13 ));
        ravenEyeInfo->temperature       = convertToFloat( modbusAsciiRegisterArrayTemp + 14, true );
        ravenEyeInfo->humidity          = convertToFloat( modbusAsciiRegisterArrayTemp + 16, true );
        ravenEyeInfo->pressure          = convertToFloat( modbusAsciiRegisterArrayTemp + 18, true );
        ravenEyeInfo->std_deviation     = convertToFloat( modbusAsciiRegisterArrayTemp + 20, true );
        ravenEyeInfo->amplitude         = convertToFloat( modbusAsciiRegisterArrayTemp + 22, true );
        ravenEyeInfo->level             = convertToFloat( modbusAsciiRegisterArrayTemp + 24, true );
        ravenEyeInfo->flowRate_Q        = convertToFloat( modbusAsciiRegisterArrayTemp + 26, true );
        ravenEyeInfo->flowRate_Q        = ( ravenEyeInfo->flowRate_Q * 3.6 ) * 24;   // convert to m^3/day
    }
    else
    {
        printf("Error on Reading ModbusASCII ReadHoldingRegisters. CODE : %d , On device : %d \n", modbusResultTemp , appConfigSt.flowMeterModbusAddress);
        //loghere
    }
    return modbusResultTemp;
}

void Set_RavenEye_ValuesOnModbusTCP( RavenEyenRegisterStruct *ravenEyeInfo, uint16_t *writtenRegisterCount )
{
    uint16_t registerCountTemp = 10;
    ModbusRegisterUnion mbusRegister;
    mbusRegister.fvalue = ravenEyeInfo->velocity_vQP;
    Modbus_SetRegisterValue( registerCountTemp++, INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++, INPUT_REGISTER_TYPE, mbusRegister.words[1] ); 

    mbusRegister.fvalue = ravenEyeInfo->velocity_vQPF;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] ); 

    mbusRegister.fvalue = ravenEyeInfo->velocity_vAVG;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );  

    mbusRegister.fvalue = ravenEyeInfo->velocity_vRAW;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );  

    mbusRegister.fvalue = ravenEyeInfo->vsn;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );

    mbusRegister.fvalue = ravenEyeInfo->snr;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );

    mbusRegister.fvalue = ravenEyeInfo->agc;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );

    mbusRegister.fvalue = ravenEyeInfo->NOT;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );

    mbusRegister.fvalue = ravenEyeInfo->temperature;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );  

    mbusRegister.fvalue = ravenEyeInfo->humidity;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] ); 

    mbusRegister.fvalue = ravenEyeInfo->pressure;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );  

    mbusRegister.fvalue = ravenEyeInfo->std_deviation;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );

    mbusRegister.fvalue = ravenEyeInfo->amplitude;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] ); 

    mbusRegister.fvalue = ravenEyeInfo->level;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );  

    mbusRegister.fvalue = ravenEyeInfo->flowRate_Q;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE, mbusRegister.words[1] );

     *(writtenRegisterCount) = registerCountTemp;
}

void Send_RavenEye_ValuesToRemoteController( RavenEyenRegisterStruct *ravenEyeInfo )
{
    uint8_t ravenEyeDataCount = 17;
    uint8_t ravenEyeDataIndex = 0;
    WebScreen_DataPackage DpArr[ ravenEyeDataCount ];
    WebScreen_DataPackage Dp;
        
    Dp.type = 1;
    
    Dp.id = PHOENIX_VQP;
    Dp.data.floatData = ravenEyeInfo->velocity_vQP;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.id = PHOENIX_VQPF;
    Dp.data.floatData = ravenEyeInfo->velocity_vQPF;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.id = PHOENIX_VRAW;
    Dp.data.floatData = ravenEyeInfo->velocity_vRAW;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.id = PHOENIX_VAVG;
    Dp.data.floatData = ravenEyeInfo->velocity_vAVG;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.id = PHOENIX_VSN;
    Dp.data.floatData = ravenEyeInfo->vsn;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.id = PHOENIX_SNR;
    Dp.data.floatData = ravenEyeInfo->snr;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.id = PHOENIX_AGC;
    Dp.data.floatData = ravenEyeInfo->agc;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.id = PHOENIX_NOT;
    Dp.data.floatData = ravenEyeInfo->NOT;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.data.floatData = ravenEyeInfo->temperature;
    Dp.id = PHOENIX_TEMPERATURE;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.data.floatData = ravenEyeInfo->pressure;
    Dp.id = PHOENIX_PRESSURE;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.data.floatData = ravenEyeInfo->humidity;
    Dp.id = PHOENIX_HUMIDITY;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    Dp.data.floatData = ravenEyeInfo->flowRate_Q;
    Dp.id = PHOENIX_FLOW_RATE;
    DpArr[ ravenEyeDataIndex++ ] = Dp;

    RC_SendDataPackage( DpArr, ravenEyeDataIndex );
}

void Get_RavenEye_ValuesToDatabaseBuffer( RavenEyenRegisterStruct *ravenEyeInfo, float *dbBuffer, uint8_t *count )
{
    uint8_t countTemp = ( *count );
    dbBuffer[ countTemp++ ] = ravenEyeInfo->velocity_vQP; 
    dbBuffer[ countTemp++ ] = ravenEyeInfo->velocity_vQPF;
    dbBuffer[ countTemp++ ] = ravenEyeInfo->velocity_vAVG;
    dbBuffer[ countTemp++ ] = ravenEyeInfo->velocity_vRAW;
    dbBuffer[ countTemp++ ] = ravenEyeInfo->temperature;  
    dbBuffer[ countTemp++ ] = ravenEyeInfo->humidity;     
    dbBuffer[ countTemp++ ] = ravenEyeInfo->pressure;     
    dbBuffer[ countTemp++ ] = ravenEyeInfo->amplitude;    
    dbBuffer[ countTemp++ ] = ravenEyeInfo->level;        
    dbBuffer[ countTemp++ ] = ravenEyeInfo->flowRate_Q;
    *(count) = countTemp;   
}


void  Print_ADS1115_Value( ADS1115InfoStruct *ads1115Info )
{

}

uint16_t Calculate_ADS1115_Values( int16_t adcDiffRead, float resistorValue, float _4mAHeightValue, float _20mAHeightValue, ADS1115InfoStruct *adsInfo )
{
    float calculationValueTemp = 0.0F;
    uint16_t retValueTemp = 0;
    float milivolt_value = 0;
    float miliampere_value = 0;
    float slopeOfConversionLine = ( 16.0F ) / ( _20mAHeightValue - _4mAHeightValue );
    milivolt_value = ( adcDiffRead ) * 0.125F; // ads1115 gain1 value
    miliampere_value = milivolt_value / resistorValue;
    adsInfo->difference = adcDiffRead;
    adsInfo->currentMa = miliampere_value;
    adsInfo->voltageMv = milivolt_value;
    if( ( miliampere_value >= 4.0F ) && ( miliampere_value <= 20.0F ) )
    {
        calculationValueTemp =  ( ( ( miliampere_value - 4 ) / slopeOfConversionLine ) + _4mAHeightValue ) ;
    }
    else if( ( miliampere_value > 3.82F ) && ( miliampere_value < 4.00F ) )
    {
    	calculationValueTemp = _4mAHeightValue;
    }
    else if( miliampere_value <= 3.80F )
    {
        calculationValueTemp = -1;
    }
    else if( ( miliampere_value > 20.00F ) && ( miliampere_value < 20.20F ) )
    {
    	calculationValueTemp = _20mAHeightValue;
    }
    else if( miliampere_value >= 20.20F )
    {
        calculationValueTemp = -1;
    }
    else
    {
        retValueTemp = -1;
        //invalid current value...
    }
    if( calculationValueTemp > 65.5 )
    {
        retValueTemp = 65500;
    }
    else
    {
        retValueTemp = calculationValueTemp * 1000;
    }
    retValueTemp = retValueTemp + appConfigSt.heightOffsetValue;
    adsInfo->height = retValueTemp;
    //printf("Difference : %d, Milivolt : %lf, Miliampere :%lf, Calculated Height( meter ) : %lf, Calculated Height(  mm ) %d \n", adcDiffRead, milivolt_value, miliampere_value, calculationValueTemp, retValueTemp );
       
    return retValueTemp;
}

float    Calculate_FlowRateWithNotchFormula( uint16_t heightParam, float notchWidthParam )
{
    const float multiplier_constant_1 = 3.6875F;
    const float multiplier_constant_2 = 2446.5755F;
    const float divider_constant = 304.8F;
    const float power_constant = 1.6F;
    const float adder_constant = 2.5F;
    
    float c_constant =  ( notchWidthParam * multiplier_constant_1 ) + adder_constant;
    float flow_rate = 0;
    flow_rate = c_constant* ( pow( ( ( (float)heightParam ) / divider_constant ), power_constant ) ) * multiplier_constant_2;
    return flow_rate;
}

void Set_ADS1115_ValuesOnModbusTCP( ADS1115InfoStruct *ads1115Info, uint16_t *writtenRegisterCount )
{
    uint16_t registerCountTemp = ( *writtenRegisterCount );
    ModbusRegisterUnion mbusRegister;
    mbusRegister.fvalue = ads1115Info->difference;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[1] );  
    mbusRegister.fvalue = ads1115Info->voltageMv;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[1] );       
    mbusRegister.fvalue = ads1115Info->currentMa;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[1] );           
    mbusRegister.fvalue = ads1115Info->height;
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[0] );
    Modbus_SetRegisterValue( registerCountTemp++ ,INPUT_REGISTER_TYPE,mbusRegister.words[1] );
    *(writtenRegisterCount) = registerCountTemp;
}

void Send_ADS1115_ValuesToRemoteController( ADS1115InfoStruct *ads1115Info )
{
    uint8_t ADS1115DataCount = 3;
    uint8_t ADS1115DataIndex = 0;
    WebScreen_DataPackage DpArr[ ADS1115DataCount ];
    WebScreen_DataPackage Dp;
    // write milivolt to buffer    
    Dp.id = ADS1115_MILIVOLT;
    Dp.data.floatData = ads1115Info->voltageMv;
    Dp.type = 1;
    DpArr[ ADS1115DataIndex++ ] = Dp;
    // write miliampere to buffer
    Dp.id = ADS1115_MILIAMPERE;
    Dp.data.floatData = ads1115Info->currentMa;
    DpArr[ ADS1115DataIndex++ ] = Dp;
    // write height to buffer
    Dp.data.floatData = ads1115Info->height;
    Dp.id = ADS1115_CALCULATED_HEIGHT_VALUE;
    DpArr[ ADS1115DataIndex++ ] = Dp;
    RC_SendDataPackage( DpArr, ADS1115DataIndex );
}

void Get_ADS1115_ValuesToDatabaseBuffer( ADS1115InfoStruct *ads1115Info, float *dbBuffer, uint8_t *count )
{
    uint8_t countTemp = ( *count );
    dbBuffer[ countTemp++ ] = ads1115Info->voltageMv;
    dbBuffer[ countTemp++ ] = ads1115Info->currentMa;
    dbBuffer[ countTemp++ ] = ads1115Info->difference;
    dbBuffer[ countTemp++ ] = ads1115Info->height;
    *(count) = countTemp;
}

void Set_EL200_ValuesOnModbusTCP( uint16_t *registerValues, uint16_t *writtenRegisterCount )
{
 	uint16_t registerCountTemp = ( *writtenRegisterCount );
	for(int k = 0; k < 16; k++)
	{
		Modbus_SetRegisterValue( registerCountTemp++, INPUT_REGISTER_TYPE, registerValues[2*k] );
    	Modbus_SetRegisterValue( registerCountTemp++, INPUT_REGISTER_TYPE, registerValues[2*k+1] ); 
	}
    *(writtenRegisterCount) = registerCountTemp;
}

float convertToFloat( uint8_t *valuesParam, bool inversedParam )
{
    float retValueTemp = 0x00;
    uint8_t fb[4];
    if( false == inversedParam ) 
    {
        fb[0] = valuesParam[0];
        fb[1] = valuesParam[1];
        fb[2] = valuesParam[2];
        fb[3] = valuesParam[3];
    }
    else
    {
        fb[0] = valuesParam[2];
        fb[1] = valuesParam[3];
        fb[2] = valuesParam[0];
        fb[3] = valuesParam[1];
    }
    memcpy(&retValueTemp, &fb, sizeof(retValueTemp));
    return retValueTemp;
}

uint32_t convertToLong( float floatValue, bool inversedParam )
{

}

void    writeChannelValuesToModbusRegisters( WebScreen_DataPackage *dataParam )
{

}


void    writeChannelValuesToDatabase( float *dbBuffer, uint8_t channelCountParam )
{
    TimeStruct_t                timeStTemp;
    DateStruct_t                dateStTemp;
    FILE                        *fileTemp;
    char                        fileNameTemp[ 128 ];
    char                        fileNewRowStrTemp[ 512 ];
    bool                        isFileExisting = false;
    RTC_GetTime( &timeStTemp );
    RTC_GetDate( &dateStTemp );

    // create file name
    sprintf( fileNameTemp, "%sChannel_Logs_%02d_%02d_%d.csv", "/home/pi/debimeter/", dateStTemp.day,dateStTemp.month,dateStTemp.year);
    
    fileTemp = fopen( (const char*)fileNameTemp, "r");
    if( fileTemp != NULL )
    {
        isFileExisting = true;
        fclose( fileTemp );
    }
    

    fileTemp = fopen((const char*)fileNameTemp,"a+");
    if ( fileTemp == NULL )
    {
        return;
    }
    if( !isFileExisting )
    {
        // put header
        fprintf( fileTemp, FILE_LOG_HEADER );
        fprintf( fileTemp, "\n");
    }
    // put time , deviceLoraId, channel Count
    sprintf( fileNewRowStrTemp,"%02d:%02d:%02d",timeStTemp.hour,timeStTemp.minute,timeStTemp.second );
    for ( int k = 0 ; k < channelCountParam ; k ++)
    {
        sprintf( fileNewRowStrTemp, "%s;%.3f", fileNewRowStrTemp, dbBuffer[ k ] );
    }
    // add data buffer and new line char
    fprintf( fileTemp, "%s",fileNewRowStrTemp );
    fprintf( fileTemp, "\n");
    // close the file..
    fclose( fileTemp );
}


///// UNUSED FUNCTIONS //////
uint8_t  Read_INA219B_Values( INA219BInfoStruct *ina219Info )
{
    uint8_t retValueTemp = 1;
    return retValueTemp;
}

void Print_INA219B_Values( INA219BInfoStruct *ina219Info )
{

}

uint8_t  Read_ADS1115_Values( ADS1115InfoStruct *ads1115Info )
{
    uint8_t retValueTemp = 1;
    return retValueTemp;
}