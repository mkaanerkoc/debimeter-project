#ifndef RAVEN_EYEN_DRIVER_H
#define RAVEN_EYEN_DRIVER_H


#include "ModbusASCIIMaster.h"
#include <stdint.h>


#define RAVEN_EYEN_TOTAL_REGISTER_COUNT         18

 #define    RAVEN_EYEN_VELOCITY_VQP_REG         1000
 #define    RAVEN_EYEN_VELOCITY_VQPF_REG        1002
 #define    RAVEN_EYEN_VELOCITY_VAVG_REG        1004
 #define    RAVEN_EYEN_VELOCITY_VRAW_REG        1006
 #define    RAVEN_EYEN_SENSOR_ID_REG            1008
 #define    RAVEN_EYEN_VSN_REG                  1009
 #define    RAVEN_EYEN_SNR_REG                  1010
 #define    RAVEN_EYEN_AGC_REG                  1012
 #define    RAVEN_EYEN_NOT_REG                  1013
 #define    RAVEN_EYEN_TEMPERATURE_REG          1014
 #define    RAVEN_EYEN_HUMIDITY_REG             1016
 #define    RAVEN_EYEN_PRESSURE_REG             1018
 #define    RAVEN_EYEN_STD_DEVIATION_REG        1020
 #define    RAVEN_EYEN_AMPLITUDE_REG            1022
 #define    RAVEN_EYEN_LEVEL_OUTPUT_REG         1024
 #define    RAVEN_EYEN_FLOW_RATE_REG            1026
 #define    RAVEN_EYEN_STATUS_REG               1028

 #define    RAVEN_EYEN_LEVEL_INPUT_REG          4000

typedef enum RavenEyenRegisterEnum
{
    RAVEN_EYEN_VELOCITY_VQP = 0,
    RAVEN_EYEN_VELOCITY_VQPF = 1,
    RAVEN_EYEN_VELOCITY_VAVG = 2,
    RAVEN_EYEN_VELOCITY_VRAW = 3,
    RAVEN_EYEN_SENSOR_IDENTIFICATION = 4,
    RAVEN_EYEN_VSN = 5,
    RAVEN_EYEN_SNR = 6,
    RAVEN_EYEN_AGC = 7,
    RAVEN_EYEN_NOT = 8,
    RAVEN_EYEN_TEMPERATURE = 9,
    RAVEN_EYEN_HUMIDITY = 10,
    RAVEN_EYEN_PRESSURE = 11,
    RAVEN_EYEN_STANDART_DEVIATION = 12,
    RAVEN_EYEN_AMPLITUDE = 13,
    RAVEN_EYEN_LEVEL_OUTPUT = 14,
    RAVEN_EYEN_FLOW_RATE = 15,
    RAVEN_EYEN_STATUS = 16,
    RAVEN_EYEN_LEVEL_INPUT = 17
}RavenEyenRegisterEnum;

typedef struct RavenEyenRegisterStruct
{
    float velocity_vQP;   // meter / second
    float velocity_vQPF; // meter / second
    float velocity_vAVG; // meter / second
    float velocity_vRAW; // meter / second
    int16_t vsn;
    float snr;
    int16_t agc;
    int16_t NOT;
    float temperature; // cantigrad
    float humidity;
    float pressure;
    float std_deviation; // standart deviation
    float amplitude;
    float level;    // meter
    float flowRate_Q; // liter/sec
		float totalFlowRate_Q; // ????? suan belirsiz...
		float dailyTotalFlowRate_Q;
}RavenEyenRegisterStruct;

extern void     Print_RavenEye_Values( RavenEyenRegisterStruct *ravenEyeInfo );
extern void     Initialize_RavenEye_Values( RavenEyenRegisterStruct *ravenEyeInfo );  
  
#endif
