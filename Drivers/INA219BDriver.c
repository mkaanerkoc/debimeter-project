#include "INA219BDriver.h"

#define RASPBERRY_COMPILE

/*private functions */
#ifdef RASPBERRY_COMPILE
void    					wireWriteRegister(uint8_t reg, uint16_t value);
bcm2835I2CReasonCodes    	wireReadRegister(uint8_t reg, uint16_t *value);
#else
void    					wireWriteRegister(uint8_t reg, uint16_t value);
bcm2835I2CReasonCodes    	wireReadRegister(uint8_t reg, uint16_t *value);
#endif

int16_t 					getBusVoltage_raw(void);
int16_t 					getShuntVoltage_raw(void);
int16_t 					getCurrent_raw(void);
int16_t 					getPower_raw(void);

/* local variables */
uint8_t ina219_i2caddr;
uint32_t ina219_calValue;
// The following multipliers are used to convert raw current and power
// values to mA and mW, taking into account the current config settings
uint32_t ina219_currentDivider_mA;
uint32_t ina219_powerMultiplier_mW;


#ifdef RASPBERRY_COMPILE
const uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
const uint8_t  slave_address = 0x40;
bcm2835I2CReasonCodes   resultOfI2Coperations;
#endif

const uint8_t  configRegisterAddress = INA219_REG_CONFIG;

/* externs */
extern ApplicationDiagnostics appDiagnosticsSt;

uint8_t INA219B_Begin( Context_Enum contextParam , uint8_t addrParam )
{
    uint16_t configRegisterValueOnPoR = 0x00;
    uint8_t retValueTemp = 0;
    ina219_i2caddr = addrParam;
    ina219_currentDivider_mA = 0;
    ina219_powerMultiplier_mW = 0;

    #ifdef RASPBERRY_COMPILE

    if( !bcm2835_init() )
    {   
        printf( "<INA219Driver> bcm2835_init failed. Are you running as root??\n" );
        //kaanfilelog...
        return 1;
    }
    if( !bcm2835_i2c_begin() )
    {
        printf( "<INA219Driver> bcm2835_i2c_begin failed. Are you running as root??\n" );
        //kaanfilelog...
        return 1;
    }

    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);

    //wireWriteRegister( 0, 0x399F );
    wireWriteRegister( INA219_REG_CONFIG, 0x399F );
    resultOfI2Coperations = wireReadRegister( INA219_REG_CONFIG, &configRegisterValueOnPoR );
    if ( resultOfI2Coperations != BCM2835_I2C_REASON_OK )
    {
        printf("<INA219Driver> Error on reading data... Reason : %d \n", resultOfI2Coperations );
        //kaanfilelog...
        return 1;
    }
    if( configRegisterValueOnPoR != 0x399F )
    {
        printf("<INA219Driver> Config Register value is not same with PoR value... 0x%x \n", configRegisterValueOnPoR );
        //kaanfilelog...
        return 1;
    }

    #endif

    INA219B_setCalibration_32V_1A();
    return retValueTemp;
}

void INA219B_setCalibration_32V_2A(void)
{
    ina219_calValue = 4096;
    ina219_currentDivider_mA = 10;  // Current LSB = 100uA per bit (1000/100 = 10)
    ina219_powerMultiplier_mW = 2;     // Power LSB = 1mW per bit (2/1)

    // Set Calibration register to 'Cal' calculated above
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Set Config register to take into account the settings above
    uint16_t config =   INA219_CONFIG_BVOLTAGERANGE_32V |
                        INA219_CONFIG_GAIN_8_320MV |
                        INA219_CONFIG_BADCRES_12BIT |
                        INA219_CONFIG_SADCRES_12BIT_1S_532US |
                        INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
    //printf("<INA219B_setCalibration_32V_2A> Config value : %X \n", config );
    wireWriteRegister( INA219_REG_CONFIG, config );
}

void INA219B_setCalibration_32V_1A(void)
{
    ina219_calValue = 10240;
    ina219_currentDivider_mA = 25;      // Current LSB = 40uA per bit (1000/40 = 25)
    ina219_powerMultiplier_mW = 1;         // Power LSB = 800mW per bit

    // Set Calibration register to 'Cal' calculated above
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Set Config register to take into account the settings above
    uint16_t config =   INA219_CONFIG_BVOLTAGERANGE_32V |
                        INA219_CONFIG_GAIN_8_320MV |
                        INA219_CONFIG_BADCRES_12BIT |
                        INA219_CONFIG_SADCRES_12BIT_1S_532US |
                        INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
    wireWriteRegister(INA219_REG_CONFIG, config);
}


void INA219B_setCalibration_16V_400mA(void)
{

  ina219_calValue = 8192;
  // Set multipliers to convert raw current/power values
  ina219_currentDivider_mA = 20;  // Current LSB = 50uA per bit (1000/50 = 20)
  ina219_powerMultiplier_mW = 1;     // Power LSB = 1mW per bit
  // Set Calibration register to 'Cal' calculated above
  wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  wireWriteRegister(INA219_REG_CONFIG, config);
}


int16_t getBusVoltage_raw()
{
  uint16_t value;
  wireReadRegister( INA219_REG_BUSVOLTAGE, &value );
  // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
  return (int16_t)((value >> 3) * 4);
}

int16_t getShuntVoltage_raw()
{
    uint16_t value;
    wireReadRegister( INA219_REG_SHUNTVOLTAGE, &value );
    return (int16_t)value;
}

int16_t getCurrent_raw()
{
    uint16_t value;

    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    wireWriteRegister( INA219_REG_CALIBRATION, ina219_calValue );
    // Now we can safely read the CURRENT register!
    wireReadRegister( INA219_REG_CURRENT, &value );
    return (int16_t)value;
}

int16_t getPower_raw()
{
    uint16_t value;
    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    wireWriteRegister( INA219_REG_CALIBRATION, ina219_calValue );

    // Now we can safely read the POWER register!
    wireReadRegister( INA219_REG_POWER, &value);
    return (int16_t)value;
}

float INA219B_getShuntVoltage_mV()
{
    int16_t value;
    value = getShuntVoltage_raw();
    return value * 0.01;
}

float INA219B_getBusVoltage_V()
{
    int16_t value = getBusVoltage_raw();
    return value * 0.001;
}

float INA219B_getCurrent_mA()
{
    float valueDec = (float)getCurrent_raw();
    valueDec /= ina219_currentDivider_mA;
    return valueDec;
}

float INA219B_getPower_mW()
{
    float valueDec = getPower_raw();
    valueDec *= ina219_powerMultiplier_mW;
    return valueDec;
}


// ### Private Functions ### //

void wireWriteRegister (uint8_t reg, uint16_t value)
{
    #ifdef RASPBERRY_COMPILE
    uint8_t highByte = ( value >> 8 );
	uint8_t lowByte = ( value & 0xFF );
	char buf[]={ reg, highByte, lowByte };
  	bcm2835_i2c_write( buf, 3 );
    #endif
}

bcm2835I2CReasonCodes wireReadRegister(uint8_t reg, uint16_t *value)
{
    #ifdef RASPBERRY_COMPILE
    bcm2835I2CReasonCodes retValueTemp = 0;
    bcm2835_i2c_write( &reg, 1 );
    retValueTemp = bcm2835_i2c_read( value, 2 );
    *(value) = ntohs( *(value) );
    return retValueTemp;
    #endif
}