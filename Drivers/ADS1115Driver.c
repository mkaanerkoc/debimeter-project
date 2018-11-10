#include "ADS1115Driver.h"

#include "stdlib.h"

// private bsp layer functions 
static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value);
static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg);


uint8_t   m_i2cAddress;
uint8_t   m_conversionDelay;
uint8_t   m_bitShift;
adsGain_t m_gain;

#ifdef RASPBERRY_COMPILE
const uint16_t clk_div_ = BCM2835_I2C_CLOCK_DIVIDER_148;
bcm2835I2CReasonCodes   resultOfI2Coperations;
#endif

uint8_t ADS1115_Begin()
{
	uint8_t retValueTemp = 0;
	m_i2cAddress = ADS1115_ADDRESS;
	m_conversionDelay = ADS1115_CONVERSIONDELAY;
	m_bitShift = 0;
   	m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
	
	#ifdef RASPBERRY_COMPILE
	if( !bcm2835_init() )
    {   
        printf( "<ADS1115Driver> bcm2835_init failed. Are you running as root??\n" );
        //kaanfilelog...
        return 1;
    }
    if( !bcm2835_i2c_begin() )
    {
        printf( "<ADS1115Driver> bcm2835_i2c_begin failed. Are you running as root??\n" );
        //kaanfilelog...
        return 1;
    }

    bcm2835_i2c_setSlaveAddress( m_i2cAddress );
    //bcm2835_i2c_setClockDivider( BCM2835_I2C_CLOCK_DIVIDER_2500 );

    uint16_t configRegValue = readRegister( m_i2cAddress, ADS1115_REG_POINTER_CONFIG );
    if( 0x8583 != configRegValue )
    {
		printf("<ADS1115Driver> Config Register Value : 0x%x \n", configRegValue );
    	return 1;
    }

    #endif
    retValueTemp = 0;
    return retValueTemp;
}

void ADS1115_SetGain( adsGain_t gain )
{
  m_gain = gain;
}

/**************************************************************************/
/*!
    @brief  Gets a gain and input voltage range
*/
/**************************************************************************/
adsGain_t ADS1115_GetGain()
{
  return m_gain;
}


void ADS1115_DENEME()
{
	uint8_t channel = 1;
	uint16_t config = 	ADS1115_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
	                    ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
	                    ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
	                    ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
	                    ADS1115_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
	                    ADS1115_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;

  	// Set single-ended input channel
  	switch (channel)
	{
	    case (0):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_0;
	      	break;
	    case (1):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_1;
	      	break;
	    case (2):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_2;
	      	break;
	    case (3):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_3;
	      	break;
  	}

  	// Set 'start single-conversion' bit
  	config |= ADS1115_REG_CONFIG_OS_SINGLE;

  	// Write config register to the ADC
 	writeRegister( m_i2cAddress, ADS1115_REG_POINTER_CONFIG, config );
 	printf("<ADS1115Driver> ADS1115_DENEME, config value : 0x%x\n", config );
 	uint16_t configRegValue = readRegister( m_i2cAddress, ADS1115_REG_POINTER_CONFIG );
 
	printf("<ADS1115Driver> ADS1115_DENEME, config Register Value : 0x%x \n", configRegValue );
}

/**************************************************************************/
/*!
    @brief  Gets a single-ended ADC reading from the specified channel
*/
/**************************************************************************/
uint16_t ADS1115_readADC_SingleEnded(uint8_t channel) 
{
  	if( channel > 3 )
  	{
	    return 0;
  	}
	  
  	// Start with default values
  	uint16_t config = 	ADS1115_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
	                    ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
	                    ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
	                    ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
	                    ADS1115_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
	                    ADS1115_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;

  	// Set single-ended input channel
  	switch (channel)
	 {
	    case (0):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_0;
	      	break;
	    case (1):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_1;
	      	break;
	    case (2):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_2;
	      	break;
	    case (3):
	      	config |= ADS1115_REG_CONFIG_MUX_SINGLE_3;
	      	break;
  	}

  	// Set 'start single-conversion' bit
  	config |= ADS1115_REG_CONFIG_OS_SINGLE;
    // Write config register to the ADC
 	  writeRegister( m_i2cAddress, ADS1115_REG_POINTER_CONFIG, config );
  	
  	// Wait for the conversion to complete
  	usleep( m_conversionDelay * 1000 );
	  // Read the conversion results
  	// Shift 12-bit results right 4 bits for the ADS1015
  	return ( readRegister( m_i2cAddress, ADS1115_REG_POINTER_CONVERT ) ) >> m_bitShift;  
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t ADS1115_readADC_Differential_0_1() 
{
  // Start with default values
  uint16_t config = ADS1115_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1115_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1115_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;
                    
  // Set channels
  config |= ADS1115_REG_CONFIG_MUX_DIFF_0_1;          // AIN0 = P, AIN1 = N

  // Set 'start single-conversion' bit
  config |= ADS1115_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(m_i2cAddress, ADS1115_REG_POINTER_CONFIG, config);

  // Wait for the conversion to complete
  delay(m_conversionDelay);

  // Read the conversion results
  uint16_t res = readRegister(m_i2cAddress, ADS1115_REG_POINTER_CONVERT) >> m_bitShift;
  if (m_bitShift == 0)
  {
    return (int16_t)res;
  }
  else
  {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF)
    {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t ADS1115_readADC_Differential_2_3() {
  // Start with default values
  uint16_t config = ADS1115_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1115_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1115_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;

  // Set channels
  config |= ADS1115_REG_CONFIG_MUX_DIFF_2_3;          // AIN2 = P, AIN3 = N

  // Set 'start single-conversion' bit
  config |= ADS1115_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(m_i2cAddress, ADS1115_REG_POINTER_CONFIG, config);

  // Wait for the conversion to complete
  usleep(m_conversionDelay*1000);

  // Read the conversion results
  uint16_t res = readRegister(m_i2cAddress, ADS1115_REG_POINTER_CONVERT) >> m_bitShift;
  if (m_bitShift == 0)
  {
    return (int16_t)res;
  }
  else
  {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF)
    {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}

/**************************************************************************/
/*!
    @brief  Sets up the comparator to operate in basic mode, causing the
            ALERT/RDY pin to assert (go from high to low) when the ADC
            value exceeds the specified threshold.
            This will also set the ADC in continuous conversion mode.
*/
/**************************************************************************/
void ADS1115_startComparator_SingleEnded(uint8_t channel, int16_t threshold)
{
  // Start with default values
  uint16_t config = ADS1115_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
                    ADS1115_REG_CONFIG_CLAT_LATCH   | // Latching mode
                    ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1115_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1115_REG_CONFIG_MODE_CONTIN  | // Continuous conversion mode
                    ADS1115_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

  // Set PGA/voltage range
  config |= m_gain;
                    
  // Set single-ended input channel
  switch (channel)
  {
    case (0):
      config |= ADS1115_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      config |= ADS1115_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      config |= ADS1115_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      config |= ADS1115_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Set the high threshold register
  // Shift 12-bit results left 4 bits for the ADS1015
  writeRegister(m_i2cAddress, ADS1115_REG_POINTER_HITHRESH, threshold << m_bitShift);

  // Write config register to the ADC
  writeRegister(m_i2cAddress, ADS1115_REG_POINTER_CONFIG, config);
}

/**************************************************************************/
/*!
    @brief  In order to clear the comparator, we need to read the
            conversion results.  This function reads the last conversion
            results without changing the config value.
*/
/**************************************************************************/
int16_t ADS1115_getLastConversionResults()
{
  // Wait for the conversion to complete
  usleep(m_conversionDelay*1000);

  // Read the conversion results
  uint16_t res = readRegister( m_i2cAddress, ADS1115_REG_POINTER_CONVERT ) >> m_bitShift;
  if (m_bitShift == 0)
  {
    return (int16_t)res;
  }
  else
  {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF)
    {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}


static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) 
{
  #ifdef RASPBERRY_COMPILE
	uint8_t highByte = ( value >> 8 );
	uint8_t lowByte = ( value & 0xFF );
	char buf[]={ reg, highByte, lowByte };
  bcm2835_i2c_write( buf, 3 );
  //bcm2835_i2c_end();
  #endif
}

static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg) 
{
  	uint16_t readByte = 0;
    #ifdef RASPBERRY_COMPILE
    bcm2835_i2c_write( &reg, 1 );
    bcm2835_i2c_read( &readByte, 2 );
    readByte = ntohs( readByte );
    //bcm2835_i2c_end();
    #endif
    return readByte;
}