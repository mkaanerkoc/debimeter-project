#include "RavenEyenDriver.h"

/*ModbusSlaveDeviceRegister ravenEyenRegisterList[ RAVEN_EYEN_TOTAL_REGISTER_COUNT ] =
{
    { "Velocity vQP",1000, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Velocity vQPF",1002, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Velocity vAVG",1004, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Velocity vRAW",1006, HOLDING_REGISTER_TYPE, FLOAT_REGISTER }, // raw surface velocity
    { "Sensor Identification",1008, HOLDING_REGISTER_TYPE, UNSIGNED16_REGISTER },
    { "VSN",1009, HOLDING_REGISTER_TYPE, SIGNED16_REGISTER }, // velocity spectrum number
    { "SNR",1010, HOLDING_REGISTER_TYPE, FLOAT_REGISTER }, // signal to noise ratio
    { "AGC",1012, HOLDING_REGISTER_TYPE, SIGNED16_REGISTER }, // automatic gain control
    { "NOT",1013, HOLDING_REGISTER_TYPE, SIGNED16_REGISTER }, // number of trials
    { "Temperature",1014, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Humidity",1016, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Pressure",1018, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Standart Deviation",1020, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Amplitude",1022, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Level Output",1024, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Flow Rate",1026, HOLDING_REGISTER_TYPE, FLOAT_REGISTER },
    { "Status",1028, HOLDING_REGISTER_TYPE, UNSIGNED16_REGISTER },
    { "Level Input",4000, HOLDING_REGISTER_TYPE, UNSIGNED16_REGISTER }
}*/

void  Print_RavenEye_Values( RavenEyenRegisterStruct *ravenEyeInfo )
{
    printf("***RavenEye Values  Begin *** \n");
    printf("velocity_vQP  Value : %.4lf \n", ravenEyeInfo->velocity_vQP );
    printf("velocity_vQPF  Value : %.4lf \n", ravenEyeInfo->velocity_vQPF );
    printf("velocity_vAVG  Value : %.4lf \n", ravenEyeInfo->velocity_vAVG );
    printf("velocity_vRAW  Value : %.4lf \n", ravenEyeInfo->velocity_vRAW );
    printf("temperature  Value : %.4lf \n", ravenEyeInfo->temperature );
    printf("humidity  Value : %.4lf \n", ravenEyeInfo->humidity );
    printf("pressure  Value : %.4lf \n", ravenEyeInfo->pressure );
    printf("amplitude  Value : %.4lf \n", ravenEyeInfo->amplitude );
    printf("level  Value : %.4lf \n", ravenEyeInfo->level );
    printf("flowRate_Q  Value : %.4lf \n", ravenEyeInfo->flowRate_Q );
    printf("***RavenEye Values  End *** \n");
    return;
}

void Initialize_RavenEye_Values( RavenEyenRegisterStruct *ravenEyeInfo )
{
    ravenEyeInfo->velocity_vQP       = -1.0F;
    ravenEyeInfo->velocity_vQPF      = -1.0F;
    ravenEyeInfo->velocity_vAVG      = -1.0F;
    ravenEyeInfo->velocity_vRAW      = -1.0F;
    
    ravenEyeInfo->temperature        = -1.0F;
    ravenEyeInfo->humidity           = -1.0F;
    ravenEyeInfo->pressure           = -1.0F;
    ravenEyeInfo->amplitude          = -1.0F;
    ravenEyeInfo->level              = -1.0F;
    ravenEyeInfo->flowRate_Q         = -1.0F;
}

void RavenEyen_ReadRegister( RavenEyenRegisterEnum registerParam , uint8_t *readValueParam)
{

}

void RavenEyen_WriteRegister( RavenEyenRegisterEnum registerParam , uint8_t *writeValueParam )
{

}
