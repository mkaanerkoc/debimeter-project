#ifndef APPLICATION_MANAGER_H
#define APPLICATION_MANAGER_H


#include <stdint.h>

typedef struct ApplicationDiagnostics
{
    uint8_t COM_PORT_STATUS;
    uint8_t INA219B_STATUS;
    uint8_t CONFIG_FILE_STATUS;
}ApplicationDiagnostics;


#endif
