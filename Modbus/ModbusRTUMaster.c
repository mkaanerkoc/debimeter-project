#include "ModbusRTUMaster.h"


uint16_t calculateCRC(uint8_t address, uint8_t* pduFrame, uint8_t pduLen);


uint8_t ModbusRTU_inputBuffer[ 128 ];
uint8_t ModbusRTU_outputBuffer[ 128 ];

uint8_t _wauchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

/* Table of CRC values for low–order byte */
uint8_t _wauchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};


ModbusMasterResult ModbusRTU_ReadInputRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam , uint16_t registerCount, uint16_t *registerValues )
{
    ModbusMasterResult retValueTemp = MODBUS_READ_SUCCESS;
    uint16_t pduLength = 0;
    uint16_t responsePduLength = ( 5 + ( registerCount * 2 ));
    uint16_t pduCrc = 0;
    uint16_t receivedPduCrc = 0;
    uint16_t totalBytesReadOnSerial = 0;

    memset( ModbusRTU_outputBuffer, 0 , sizeof( ModbusRTU_outputBuffer ));
    memset( ModbusRTU_inputBuffer,  0 , sizeof( ModbusRTU_inputBuffer  ));
    ModbusMaster_PrepareReadInputRegistersPDU( deviceIDParam, registerAddressParam, registerCount, ModbusRTU_outputBuffer, &pduLength );
    pduCrc = calculateCRC( deviceIDParam, ModbusRTU_outputBuffer + 1, ( pduLength - 1) );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc >>  8 );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc &   0xFF );
    
    #ifndef DEBUG_PRINTF
    printf("ModbusRTU_ReadInputRegisters msg: \n");
    for( int i =  0 ; i < pduLength ; i++ )
    {
        printf(" %d",ModbusRTU_outputBuffer[ i ] );
    }
    printf("\n");
    #endif

    SerialPort_Write( ModbusRTU_outputBuffer, pduLength );
    totalBytesReadOnSerial = SerialPort_Read( ModbusRTU_inputBuffer, responsePduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == responsePduLength )
        {
            pduCrc = calculateCRC( deviceIDParam, ( ModbusRTU_inputBuffer + 1 ), ( responsePduLength - 3) );
            receivedPduCrc = ( ModbusRTU_inputBuffer[responsePduLength - 2] <<8 ) | ( ModbusRTU_inputBuffer[ responsePduLength -1 ] );
            if ( pduCrc != receivedPduCrc )
            {
                retValueTemp = MODBUS_FRAME_CHECK_ERROR;
                return retValueTemp;
            }

            if( ModbusRTU_inputBuffer[0] != deviceIDParam )
            {
                // id mismatch..
                retValueTemp = MODBUS_WRONG_ID_ERROR;
                return retValueTemp;
            }

            if( ( ModbusRTU_inputBuffer[1] > 0x80 ) && ( ( ModbusRTU_inputBuffer[1] < 0x91 ) ) )
            {
                //modbus error
                printf("modbusError Code : %X, %X \n", ModbusRTU_inputBuffer[ 1 ], ( ModbusRTU_inputBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }

            if ( ModbusRTU_inputBuffer[1] != READ_INPUT_REGISTERS_FCODE )
            {
                // fcode mismatch mq
                retValueTemp = MODBUS_WRONG_FCODE_ERROR;
                return retValueTemp;
            }

            //printf( "ModbusRTU_ReadInputRegisters: register Value : %d\n", ( ModbusRTU_inputBuffer[3] << 8 ) | ModbusRTU_inputBuffer[4] );
            // kaanbak: modbus error code 'lari eklemek gerek..
            for ( int i = 0 ; i < registerCount ; i++)
            {
                registerValues[ i ] = ( ( ModbusRTU_inputBuffer[3 + i * 2 ] << 8 ) | ( ModbusRTU_inputBuffer[4 + i * 2 ])  );
            }
        }
        else
        {

        }
    }
    else
    {
        printf("<ModbusRTU> Response Read time out oldu..\n");
    }
    return retValueTemp;
}

ModbusMasterResult ModbusRTU_ReadHoldingRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam , uint16_t registerCount, uint16_t *registerValues)
{
    ModbusMasterResult retValueTemp = MODBUS_READ_SUCCESS;
    uint16_t pduLength = 0;
    uint16_t pduCrc = 0;
    uint16_t receivedPduCrc = 0;
    uint16_t totalBytesReadOnSerial = 0;
    uint16_t responsePduLength = ( 5 + ( registerCount * 2 ));

    memset( ModbusRTU_outputBuffer, 0 , sizeof( ModbusRTU_outputBuffer ));
    memset( ModbusRTU_inputBuffer, 0 , sizeof( ModbusRTU_inputBuffer ));
    ModbusMaster_PrepareReadHoldingRegistersPDU( deviceIDParam, registerAddressParam, registerCount, ModbusRTU_outputBuffer, &pduLength );
    pduCrc = calculateCRC( deviceIDParam, ModbusRTU_outputBuffer + 1, ( pduLength - 1) );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc >>  8 );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc &   0xFF );

    #ifdef DEBUG_PRINTF
    printf("rtu msg: \n");
    for( int i =  0 ; i < pduLength ; i++ )
    {
        printf(" %x",ModbusRTU_outputBuffer[ i ] );
    }
    printf("\n");
    #endif

    SerialPort_Write( ModbusRTU_outputBuffer, pduLength );
    totalBytesReadOnSerial = SerialPort_Read( ModbusRTU_inputBuffer, responsePduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == responsePduLength )
        {
            pduCrc = calculateCRC( deviceIDParam, ( ModbusRTU_inputBuffer + 1 ), ( responsePduLength - 3) );
            receivedPduCrc = ( ModbusRTU_inputBuffer[responsePduLength - 2] <<8 ) | ( ModbusRTU_inputBuffer[ responsePduLength -1 ] );
            if( pduCrc != receivedPduCrc )
            {
                retValueTemp = MODBUS_FRAME_CHECK_ERROR;
                return retValueTemp;
            }
            if( ModbusRTU_inputBuffer[0] != deviceIDParam )
            {
                 // id mismatch..
                retValueTemp = MODBUS_WRONG_ID_ERROR;
                return retValueTemp;
            }
            
            if( ( ModbusRTU_inputBuffer[1] > 0x80 ) && ( ( ModbusRTU_inputBuffer[1] < 0x91 ) ) )
            {
                //modbus error
                printf("modbusError Code : %X, %X \n", ModbusRTU_inputBuffer[ 1 ], ( ModbusRTU_inputBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }

            if( ModbusRTU_inputBuffer[1] != READ_INPUT_REGISTERS_FCODE )
            {
                // fcode mismatch mq
                retValueTemp = MODBUS_WRONG_FCODE_ERROR;
                return retValueTemp;
            }

            //printf( "ModbusRTU_ReadInputRegisters: register Value : %d\n", ( ModbusRTU_inputBuffer[3] << 8 ) | ModbusRTU_inputBuffer[4] );
            // kaanbak: modbus error code 'lari eklemek gerek..
            for( int i = 0 ; i < registerCount ; i++)
            {
                registerValues[ i ] = ( ( ModbusRTU_inputBuffer[3 + i * 2 ] << 8 ) | ( ModbusRTU_inputBuffer[4 + i * 2 ])  );
            }
        }
        else
        {
            // modbus error...
        }
    }
    else
    {
        //KAANKESIN BAK SUAN CALISMIYOR BURASI...timeout oldu...
        printf("<ModbusRTU> Response Read time out oldu..\n");
    }
   
    //kaantodo


    return retValueTemp;
}

ModbusMasterResult ModbusRTU_WriteHoldingRegister( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t valueParam )
{
    uint16_t pduLength = 0;
    uint16_t pduCrc = 0;
    uint16_t receivedPduCrc = 0;
    uint16_t totalBytesReadOnSerial = 0;

    ModbusMasterResult retValueTemp = MODBUS_WRITE_SUCCESS;

    memset( ModbusRTU_outputBuffer, 0 , sizeof( ModbusRTU_outputBuffer ));
    memset( ModbusRTU_inputBuffer, 0 , sizeof( ModbusRTU_inputBuffer ));


    ModbusMaster_PrepareWriteHoldingRegisterPDU( deviceIDParam, registerAddressParam, valueParam, ModbusRTU_outputBuffer, &pduLength );
    pduCrc = calculateCRC( deviceIDParam, ModbusRTU_outputBuffer + 1, ( pduLength - 1) );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc >>  8 );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc &   0xFF );
    
    uint16_t responsePduLength = responsePduLength = pduLength;
    
    #ifndef DEBUG_PRINTF
    printf("ModbusRTU_WriteHoldingRegister msg: \n");
    for( int i =  0 ; i < pduLength ; i++ )
    {
        printf(" %d",ModbusRTU_outputBuffer[ i ] );
    }
    printf("\n");
    #endif
    
    SerialPort_Write( ModbusRTU_outputBuffer, pduLength );
    
    totalBytesReadOnSerial = SerialPort_Read( ModbusRTU_inputBuffer, responsePduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == responsePduLength )
        {
            pduCrc = calculateCRC( deviceIDParam, ( ModbusRTU_inputBuffer + 1 ), ( responsePduLength - 3) );
            receivedPduCrc = ( ModbusRTU_inputBuffer[responsePduLength - 2] <<8 ) | ( ModbusRTU_inputBuffer[ responsePduLength -1 ] );
            if( pduCrc != receivedPduCrc )
            {
                retValueTemp = MODBUS_FRAME_CHECK_ERROR;
                return retValueTemp;
            }
            if( ModbusRTU_inputBuffer[0] != deviceIDParam )
            {
                // id mismatch uyusmazligi..
                printf("<ModbusRTU_WriteHoldingRegister> Response id mismatch uyusmazligi...\n");
                retValueTemp = MODBUS_WRONG_ID_ERROR;
                return retValueTemp;
            }
            
            if( ( ModbusRTU_inputBuffer[1] > 0x80 ) && ( ( ModbusRTU_inputBuffer[1] < 0x91 ) ) )
            {
                //modbus error
                printf("modbusError Code : %X, %X \n", ModbusRTU_inputBuffer[ 1 ], ( ModbusRTU_inputBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }

            if( ModbusRTU_inputBuffer[1] != WRITE_HOLDING_REGISTER_FCODE )
            {
                // function code mismatch..
                printf("<ModbusRTU_WriteHoldingRegister> Response function code mismatch.. uyusmazligi...\n");
                // fcode mismatch mq
                retValueTemp = MODBUS_WRONG_FCODE_ERROR;
                return retValueTemp;
            }
        }
        else
        {
            //modbus error..
        }
    }
    else
    {
        printf("<ModbusRTU_WriteHoldingRegister> Response Read time out oldu..\n");
        retValueTemp = MODBUS_RESPONSE_TIMEOUT;
    }

    return retValueTemp;
}

ModbusMasterResult ModbusRTU_WriteHoldingRegisters(  uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t *valuesParam, uint8_t registerCount )
{
    uint16_t pduLength = 0;
    uint16_t pduCrc = 0;
    uint16_t receivedPduCrc = 0;
    uint16_t totalBytesReadOnSerial = 0;
    uint16_t responsePduLength = 0;
    ModbusMasterResult retValueTemp = MODBUS_WRITE_SUCCESS;

    memset( ModbusRTU_outputBuffer, 0 , sizeof( ModbusRTU_outputBuffer ));
    memset( ModbusRTU_inputBuffer, 0 , sizeof( ModbusRTU_inputBuffer ));


    ModbusMaster_PrepareWriteMultiHoldingRegistersPDU( deviceIDParam, registerAddressParam, valuesParam, registerCount, ModbusRTU_outputBuffer, &pduLength );
    pduCrc = calculateCRC( deviceIDParam, ModbusRTU_outputBuffer + 1, ( pduLength - 1) );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc >>  8 );
    ModbusRTU_outputBuffer[ pduLength++ ] = ( pduCrc &   0xFF );
    
    responsePduLength = 8;
    SerialPort_Write( ModbusRTU_outputBuffer, pduLength );
    totalBytesReadOnSerial = SerialPort_Read( ModbusRTU_inputBuffer, responsePduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == responsePduLength )
        {

        }
        else
        {

        }
    }
    else
    {

    }

    return retValueTemp; 
    //KAANTODO:
}

uint16_t calculateCRC(uint8_t address, uint8_t* pduFrame, uint8_t pduLen)
{
    uint8_t CRCHi = 0xFF, CRCLo = 0x0FF, Index;
    Index = CRCHi ^ address;
    CRCHi = CRCLo ^ _wauchCRCHi[Index];
    CRCLo = _wauchCRCLo[Index];

    while (pduLen--)
    {
        Index = CRCHi ^ *pduFrame++;
        CRCHi = CRCLo ^ _wauchCRCHi[Index];
        CRCLo = _wauchCRCLo[Index];
    }

    return (CRCHi << 8) | CRCLo;
}
