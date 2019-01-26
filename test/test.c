#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "ConfigurationManager.h"

float    Calculate_FlowRateWithNotchFormula( uint16_t heightParam, float notchWidthParam );


int main(void)
{
	float result = Calculate_FlowRateWithNotchFormula( 480, 15 ) / 24;
	printf("Result : %.2f\n", result );
	result = Calculate_FlowRateWithNotchFormula( 480, 5 ) / 24;
	printf("Result : %.2f\n", result );

	ConfigurationManager_Begin("test_config_file.ini");
	ConfigurationManager_PrintConfiguration( &appConfigSt );
	return 0;
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