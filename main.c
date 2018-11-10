#include <stdio.h>
#include <stdint.h>
#include <bcm2835.h>


int Initialize_BSP_Layer( int i2cInit, int uartInit );


uint16_t 					clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
uint8_t 						slave_address = 0x40;

char 							buf[ 16 ];
int 							i;

bcm2835I2CReasonCodes 	result;

int main(void)
{
	printf("<Debimeter Application> Starting application...\n");
	int resultOfBspTemp = Initialize_BSP_Layer( 1, 0 );	
	if ( 0 == resultOfBspTemp )
	{
		printf("<Debimeter Application> Initialization of BSP is successfull...\n"); // may be set variable here
	}

	return 0;
}


int Initialize_BSP_Layer( int i2cInit, int uartInit ) 
{

	if ( !bcm2835_init() )
	{	
		printf( "<INA219Driver> bcm2835_init failed. Are you running as root??\n" );
   	//kaanfilelog...
   	return 1;
 	}
      
 	// I2C begin if specified    
 	if( i2cInit == 1 )
 	{
  		if ( !bcm2835_i2c_begin() )
		{
        	printf( "<INA219Driver> bcm2835_i2c_begin failed. Are you running as root??\n" );
        	//kaanfilelog...
        	return 1;
		}
 	}

 	bcm2835_i2c_setSlaveAddress(slave_address);
   bcm2835_i2c_setClockDivider(clk_div);

   uint16_t configRegisterValueOnPoR = 0x00;
	result = bcm2835_i2c_read( &configRegisterValueOnPoR, 2 );
	if ( result != BCM2835_I2C_REASON_OK )
	{
		printf("<INA219Driver> Error on reading data... Reason : %d \n", result );
		//kaanfilelog...
		return 1;
	}
	if( configRegisterValueOnPoR != 0x9F39 )
	{
		printf("<INA219Driver> Config Register value is not same with PoR value... 0x%x \n", configRegisterValueOnPoR );
		//kaanfilelog...
		return 1;
	}
	
 	return 0;
 	// go from here to read ina219 const registers..
}