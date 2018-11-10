#include "ModbusASCIIMaster.h"


//void convertModbusPDUfromRTUtoASCII ( uint8_t *pduBufferParam, uint16_t pduLengthParam );
//void convertModbusPDUfromASCIItoRTU ( uint8_t *pduBufferParam, uint16_t pduLengthParam );

#define         MODBUS_ASCII_BUFFER_SIZE    512


bool            checkForValidNibbleValue( uint8_t nibbleParam );
bool            convertModbusFramefromRTUtoASCII( uint8_t *inputPduParam, uint16_t inputPduLenghtParam, uint8_t *outputPduParam, uint16_t *outputPduLengthParam );
bool            convertModbusFramefromASCIItoRTU( uint8_t *inputPduParam, uint16_t inputPduLenghtParam, uint8_t *outputPduParam, uint16_t *outputPduLengthParam );
uint8_t         calculateLRC( uint8_t *pduPayloadParam, uint16_t lengthParam );

uint8_t         ModbusASCII_inputBuffer[ MODBUS_ASCII_BUFFER_SIZE ];
uint8_t         ModbusASCII_outputBuffer[ MODBUS_ASCII_BUFFER_SIZE ];
uint8_t         ModbusASCII_tempBuffer[ MODBUS_ASCII_BUFFER_SIZE ]; // holds temporary converting pdu buf.

ModbusMasterResult ModbusASCII_ReadInputRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam, uint8_t registerCount, uint16_t *registerValues )
{
    ModbusMasterResult retValueTemp = MODBUS_READ_SUCCESS;

    uint16_t tx_rtuPduLength = 0, rx_rtuPduLength = 0;
    uint16_t tx_asciiPduLength = 0, rx_asciiPduLength = 0;
    uint8_t tx_pduLRC = 0, rx_pduLRC = 0;
    bool convertResultTemp = false;
    uint16_t totalBytesReadOnSerial = 0;

    rx_rtuPduLength     = ( 3 + ( registerCount * 2 )) + 2; // 2 byte crc; //kaanbak check here...
    rx_asciiPduLength   = ( 1 + ( ( rx_rtuPduLength - 1 ) * 2 ) + 2 );

    memset( ModbusASCII_outputBuffer    , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_inputBuffer     , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_tempBuffer      , 0 , MODBUS_ASCII_BUFFER_SIZE );

    ModbusMaster_PrepareReadInputRegistersPDU( deviceIDParam, registerAddressParam, registerCount, ModbusASCII_tempBuffer, &tx_rtuPduLength );
    tx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, tx_rtuPduLength ); // calculate LRC before converting PDU to ASCII
    ModbusASCII_tempBuffer[ tx_rtuPduLength++ ] = tx_pduLRC; // we converting PDU byte to ASCII representation also
    convertModbusFramefromRTUtoASCII( ModbusASCII_tempBuffer, tx_rtuPduLength, ModbusASCII_outputBuffer, &tx_asciiPduLength );

    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0D; // Carriage Return byte
    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0A; // Line Feed byte

    #if 0
    printf("<ModbusASCII_ReadInputRegisters> ASCII Buffer Length : %d, Buffer -> ", asciiPduLength );
    for( int k = 0; k < asciiPduLength; k++ )
    {
        printf(" %c", ( char ) ModbusASCII_outputBuffer[ k ] );
    }
    printf("\n"); 
    #endif

    SerialPort_Write( ModbusASCII_outputBuffer, tx_asciiPduLength );

    totalBytesReadOnSerial = SerialPort_Read( ModbusASCII_inputBuffer, rx_asciiPduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == rx_asciiPduLength )
        {
            memset( ModbusASCII_tempBuffer, 0, MODBUS_ASCII_BUFFER_SIZE );
            #if 0
            printf("<ModbusASCII_ReadInputRegisters> Received Bytes : ");
            for( int k = 0 ; k < rx_asciiPduLength ; k++)
            {
                printf(" %X", ModbusASCII_inputBuffer[ k ] );
            } 
            printf("\n");
            #endif 
            convertResultTemp = convertModbusFramefromASCIItoRTU( ModbusASCII_inputBuffer, rx_asciiPduLength, ModbusASCII_tempBuffer, &rx_rtuPduLength );
            if( false == convertResultTemp )
            {
                retValueTemp = MODBUS_ERROR_ON_PARSING_RESP;
                return retValueTemp;
            }
            #if 0
            printf("<ModbusASCII_ReadInputRegisters> Converted PDU : ");
            for( int k = 0 ; k < rx_rtuPduLength ; k++)
            {
                printf(" %X", ModbusASCII_tempBuffer[ k ] );
            } 
            printf("\n");
            #endif 
            rx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, ( rx_rtuPduLength - 1 ) );

            if( rx_pduLRC != ModbusASCII_tempBuffer[ rx_rtuPduLength -1 ] )
            {
                retValueTemp = MODBUS_FRAME_CHECK_ERROR;
                //printf("<ModbusASCII_ReadInputRegisters> LRC's are not same.. CALCULATED LRC : %X, RX LRC : %X\n", rx_pduLRC, ModbusASCII_tempBuffer[ rx_rtuPduLength -1 ] );
                return retValueTemp;
            }

            if( ModbusASCII_tempBuffer[0] != deviceIDParam )
            {
                // id mismatch..
                retValueTemp = MODBUS_WRONG_ID_ERROR;
                return retValueTemp;
            }

            if( ( ModbusASCII_tempBuffer[1] > 0x80 ) && ( ( ModbusASCII_tempBuffer[1] < 0x91 ) ) )
            {
                //modbus error
                printf("modbusError Code : %X, %X \n", ModbusASCII_tempBuffer[ 1 ], ( ModbusASCII_tempBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }

            if ( ModbusASCII_tempBuffer[1] != READ_INPUT_REGISTERS_FCODE )
            {
                // fcode mismatch mq
                retValueTemp = MODBUS_WRONG_FCODE_ERROR;
                return retValueTemp;
            }

            //printf( "ModbusRTU_ReadInputRegisters: register Value : %d\n", ( ModbusRTU_inputBuffer[3] << 8 ) | ModbusRTU_inputBuffer[4] );
            for ( int i = 0 ; i < registerCount ; i++)
            {
                registerValues[ i ] = ( ( ModbusASCII_tempBuffer[ 3 + i * 2 ] << 8 ) | ( ModbusASCII_tempBuffer[4 + i * 2 ])  );
            } 
        }
        else
        {   // kaanbak: modbus error code 'lari eklemek gerek..
            printf("An Error Occured on ModbusASCII_ReadInputRegisters ,totalBytesReadOnSerial : %d \n", totalBytesReadOnSerial );
            convertResultTemp = convertModbusFramefromASCIItoRTU( ModbusASCII_inputBuffer, totalBytesReadOnSerial, ModbusASCII_tempBuffer, &rx_rtuPduLength );
            if( false == convertResultTemp )
            {
                retValueTemp = MODBUS_ERROR_ON_PARSING_RESP;
                return retValueTemp;
            }
            printf("<ModbusASCII_ReadInputRegisters> Converted PDU : ");
            for( int k = 0 ; k < rx_rtuPduLength ; k++)
            {
                printf(" %X", ModbusASCII_tempBuffer[ k ] );
            } 
            printf("\n");
            if( ModbusASCII_tempBuffer[ 1 ] > 0x80 )
            {
                printf("modbusError Code : %X, %X \n", ModbusASCII_tempBuffer[ 1 ], ( ModbusASCII_tempBuffer[ 1 ] - 0x80 ) );
            }
            return MODBUS_RESPONSE_TIMEOUT;
        }   
    }
    else
    {
        retValueTemp = MODBUS_RESPONSE_TIMEOUT;
        return retValueTemp;
    }

    return retValueTemp;
}

ModbusMasterResult ModbusASCII_ReadHoldingRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam, uint8_t registerCount, uint16_t *registerValues )
{
    ModbusMasterResult retValueTemp = MODBUS_READ_SUCCESS;

    uint16_t tx_rtuPduLength = 0, rx_rtuPduLength = 0;
    uint16_t tx_asciiPduLength = 0, rx_asciiPduLength = 0;
    uint8_t tx_pduLRC = 0, rx_pduLRC = 0;
    bool convertResultTemp = false;
    uint16_t totalBytesReadOnSerial = 0;
    
    rx_rtuPduLength     = ( 3 + ( registerCount * 2 )) + 2; // 2 byte crc; //kaanbak check here...
    rx_asciiPduLength   = ( 1 + ( ( rx_rtuPduLength - 1 ) * 2 ) + 2 );

    memset( ModbusASCII_outputBuffer    , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_inputBuffer     , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_tempBuffer      , 0 , MODBUS_ASCII_BUFFER_SIZE );

    ModbusMaster_PrepareReadHoldingRegistersPDU( deviceIDParam, registerAddressParam, registerCount, ModbusASCII_tempBuffer, &tx_rtuPduLength );
    tx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, tx_rtuPduLength ); // calculate LRC before converting PDU to ASCII
    ModbusASCII_tempBuffer[ tx_rtuPduLength++ ] = tx_pduLRC; // we converting PDU byte to ASCII representation also
    convertResultTemp = convertModbusFramefromRTUtoASCII( ModbusASCII_tempBuffer, tx_rtuPduLength, ModbusASCII_outputBuffer, &tx_asciiPduLength );
    
    if( false == convertResultTemp )
    {
        retValueTemp = MODBUS_ERROR_ON_CREATING_MSG;
        return retValueTemp;
    }

    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0D; // Carriage Return byte
    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0A; // Line Feed byte
    
    #if 0
    printf("<ModbusASCII_ReadHoldingRegisters> ASCII Buffer Length : %d, Buffer -> ", tx_asciiPduLength );
    for( int k = 0; k < tx_asciiPduLength; k++ )
    {
        printf(" %02x", ( char ) ModbusASCII_outputBuffer[ k ] );
    }
    printf("\n"); 
    #endif
    // send to serial bus
    
    SerialPort_Write( ModbusASCII_outputBuffer, tx_asciiPduLength );
    totalBytesReadOnSerial = SerialPort_Read( ModbusASCII_inputBuffer, rx_asciiPduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == rx_asciiPduLength )
        {
            memset( ModbusASCII_tempBuffer, 0, MODBUS_ASCII_BUFFER_SIZE );
            convertResultTemp = convertModbusFramefromASCIItoRTU( ModbusASCII_inputBuffer, rx_asciiPduLength, ModbusASCII_tempBuffer, &rx_rtuPduLength );
            if( false == convertResultTemp )
            {
                retValueTemp = MODBUS_ERROR_ON_PARSING_RESP;
                return retValueTemp;
            }

            rx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, ( rx_rtuPduLength - 1 ) );

            if( rx_pduLRC != ModbusASCII_tempBuffer[ rx_rtuPduLength -1 ] )
            {
                retValueTemp = MODBUS_FRAME_CHECK_ERROR;
                return retValueTemp;
            }

            if( ModbusASCII_tempBuffer[0] != deviceIDParam )
            {
                // id mismatch..
                retValueTemp = MODBUS_WRONG_ID_ERROR;
                return retValueTemp;
            }
            if( ( ModbusASCII_tempBuffer[1] > 0x80 ) && ( ( ModbusASCII_tempBuffer[1] < 0x91 ) ) )
            {
                //modbus error
                printf("modbusError Code : %X, %X \n", ModbusASCII_tempBuffer[ 1 ], ( ModbusASCII_tempBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }
            if( ModbusASCII_tempBuffer[1] != READ_HOLDING_REGISTERS_FCODE )
            {
                // fcode mismatch mq
                retValueTemp = MODBUS_WRONG_FCODE_ERROR;
                return retValueTemp;
            }

            //printf( "ModbusRTU_ReadInputRegisters: register Value : %d\n", ( ModbusRTU_inputBuffer[3] << 8 ) | ModbusRTU_inputBuffer[4] );
            // kaanbak: modbus error code 'lari eklemek gerek..
            for ( int i = 0 ; i < registerCount ; i++)
            {
                registerValues[ i ] = ( ( ModbusASCII_tempBuffer[3 + i * 2 ] << 8 ) | ( ModbusASCII_tempBuffer[4 + i * 2 ])  );
            }
        }
        else
        {
            // kaanbak: modbus error code 'lari eklemek gerek..
            convertResultTemp = convertModbusFramefromASCIItoRTU( ModbusASCII_inputBuffer, totalBytesReadOnSerial, ModbusASCII_tempBuffer, &rx_rtuPduLength );
            if( false == convertResultTemp )
            {
                retValueTemp = MODBUS_ERROR_ON_PARSING_RESP;
                return retValueTemp;
            }
            #if 0
            printf("<ModbusASCII_ReadHoldingRegisters> Converted PDU : ");
            for( int k = 0 ; k < rx_rtuPduLength ; k++)
            {
                printf(" %X", ModbusASCII_tempBuffer[ k ] );
            } 
            printf("\n");
            #endif 
            if( ( ModbusASCII_tempBuffer[1] > 0x80 ) && ( ( ModbusASCII_tempBuffer[1] < 0x91 ) ) )
            {
                printf("modbusError Code : %X, %X \n", ModbusASCII_tempBuffer[ 1 ], ( ModbusASCII_tempBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }
            return MODBUS_RESPONSE_TIMEOUT;
        }
    }
    else
    {
        retValueTemp = MODBUS_RESPONSE_TIMEOUT;
        return retValueTemp;
    }

    return retValueTemp;
}

ModbusMasterResult ModbusASCII_WriteHoldingRegister( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t valueParam )
{
    ModbusMasterResult retValueTemp = MODBUS_WRITE_SUCCESS;

    uint16_t tx_rtuPduLength = 0, rx_rtuPduLength = 0;
    uint16_t tx_asciiPduLength = 0, rx_asciiPduLength = 0;
    uint8_t tx_pduLRC = 0, rx_pduLRC = 0;
    bool convertResultTemp = false;
    uint16_t totalBytesReadOnSerial = 0;

    memset( ModbusASCII_outputBuffer    , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_inputBuffer     , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_tempBuffer      , 0 , MODBUS_ASCII_BUFFER_SIZE );

    ModbusMaster_PrepareWriteHoldingRegisterPDU( deviceIDParam, registerAddressParam, valueParam, ModbusASCII_tempBuffer, &tx_rtuPduLength );
    tx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, tx_rtuPduLength ); // calculate LRC before converting PDU to ASCII
    ModbusASCII_tempBuffer[ tx_rtuPduLength++ ] = tx_pduLRC; // we converting PDU byte to ASCII representation also
    convertResultTemp = convertModbusFramefromRTUtoASCII( ModbusASCII_tempBuffer, tx_rtuPduLength, ModbusASCII_outputBuffer, &tx_asciiPduLength );
    
    rx_rtuPduLength     = tx_rtuPduLength; // 2 byte crc; //kaanbak check here...
    rx_asciiPduLength   = ( 1 + ( rx_rtuPduLength * 2 ) + 2 );

    if( false == convertResultTemp )
    {
        retValueTemp = MODBUS_ERROR_ON_CREATING_MSG;
        return retValueTemp;
    }

    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0D; // Carriage Return byte
    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0A; // Line Feed byte
    
    #if 0
    printf("<ModbusASCII_ReadHoldingRegisters> ASCII Buffer Length : %d, Buffer -> ", tx_asciiPduLength );
    for( int k = 0; k < tx_asciiPduLength; k++ )
    {
        printf(" %02x", ( char ) ModbusASCII_outputBuffer[ k ] );
    }
    printf("\n"); 
    #endif

    SerialPort_Write( ModbusASCII_outputBuffer, tx_asciiPduLength ); // send to serial bus

    totalBytesReadOnSerial = SerialPort_Read( ModbusASCII_inputBuffer, rx_asciiPduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == rx_asciiPduLength )
        {
            memset( ModbusASCII_tempBuffer, 0, MODBUS_ASCII_BUFFER_SIZE );
            convertResultTemp = convertModbusFramefromASCIItoRTU( ModbusASCII_inputBuffer, rx_asciiPduLength, ModbusASCII_tempBuffer, &rx_rtuPduLength );
            if( false == convertResultTemp )
            {
                retValueTemp = MODBUS_ERROR_ON_PARSING_RESP;
                return retValueTemp;
            }

            rx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, ( rx_rtuPduLength - 1 ) );

            if( rx_pduLRC != ModbusASCII_tempBuffer[ rx_rtuPduLength -1 ] )
            {
                retValueTemp = MODBUS_FRAME_CHECK_ERROR;
                return retValueTemp;
            }

            if( ModbusASCII_tempBuffer[0] != deviceIDParam )
            {
                // id mismatch..
                retValueTemp = MODBUS_WRONG_ID_ERROR;
                return retValueTemp;
            }

            if( ( ModbusASCII_tempBuffer[1] > 0x80 ) && ( ( ModbusASCII_tempBuffer[1] < 0x91 ) ) )
            {
                //modbus error
                printf("modbusError Code : %X, %X \n", ModbusASCII_tempBuffer[ 1 ], ( ModbusASCII_tempBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }

            if ( ModbusASCII_tempBuffer[1] != WRITE_HOLDING_REGISTER_FCODE )
            {
                // fcode mismatch mq
                retValueTemp = MODBUS_WRONG_FCODE_ERROR;
                return retValueTemp;
            }
        }
        else
        {

        }
    }
    else
    {
        retValueTemp = MODBUS_RESPONSE_TIMEOUT;
        return retValueTemp;
    }

    return retValueTemp;
}

ModbusMasterResult ModbusASCII_WriteHoldingRegisters( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t *valuesParam, uint8_t registerCount )
{
    ModbusMasterResult retValueTemp = MODBUS_WRITE_SUCCESS;

    uint16_t tx_rtuPduLength = 0, rx_rtuPduLength = 0;
    uint16_t tx_asciiPduLength = 0, rx_asciiPduLength = 0;
    uint8_t tx_pduLRC = 0, rx_pduLRC = 0;
    bool convertResultTemp = false;
    uint16_t totalBytesReadOnSerial = 0;
    
    rx_rtuPduLength     = 8; //fixed
    rx_asciiPduLength   = ( 1 + ( ( rx_rtuPduLength - 1 ) * 2 ) + 2 );

    memset( ModbusASCII_outputBuffer    , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_inputBuffer     , 0 , MODBUS_ASCII_BUFFER_SIZE );
    memset( ModbusASCII_tempBuffer      , 0 , MODBUS_ASCII_BUFFER_SIZE );

    ModbusMaster_PrepareWriteMultiHoldingRegistersPDU( deviceIDParam, registerAddressParam, valuesParam, registerCount, ModbusASCII_tempBuffer, &tx_rtuPduLength );
    tx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, tx_rtuPduLength ); // calculate LRC before converting PDU to ASCII
    ModbusASCII_tempBuffer[ tx_rtuPduLength++ ] = tx_pduLRC; // we converting PDU byte to ASCII representation also
    convertResultTemp = convertModbusFramefromRTUtoASCII( ModbusASCII_tempBuffer, tx_rtuPduLength, ModbusASCII_outputBuffer, &tx_asciiPduLength );
    
    if( false == convertResultTemp )
    {
        retValueTemp = MODBUS_ERROR_ON_CREATING_MSG;
        return retValueTemp;
    }

    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0D; // Carriage Return byte
    ModbusASCII_outputBuffer[ tx_asciiPduLength++ ] = 0x0A; // Line Feed byte

    #if 0
    printf("<ModbusASCII_WriteHoldingRegisters> ASCII Buffer Length : %d, Buffer -> ", tx_asciiPduLength );
    for( int k = 0; k < tx_asciiPduLength; k++ )
    {
        printf(" %02x", ( char ) ModbusASCII_outputBuffer[ k ] );
    }
    printf("\n"); 
    #endif

    SerialPort_Write( ModbusASCII_outputBuffer, tx_asciiPduLength );    // send to serial bus
    totalBytesReadOnSerial = SerialPort_Read( ModbusASCII_inputBuffer, rx_asciiPduLength, 1 );
    if( totalBytesReadOnSerial > 0 )
    {
        if( totalBytesReadOnSerial == rx_asciiPduLength )
        {
            memset( ModbusASCII_tempBuffer, 0, MODBUS_ASCII_BUFFER_SIZE );
            convertResultTemp = convertModbusFramefromASCIItoRTU( ModbusASCII_inputBuffer, rx_asciiPduLength, ModbusASCII_tempBuffer, &rx_rtuPduLength );
            if( false == convertResultTemp )
            {
                retValueTemp = MODBUS_ERROR_ON_PARSING_RESP;
                return retValueTemp;
            }

            #if 0
            printf("<ModbusASCII_WriteHoldingRegisters> Converted PDU : ");
            for( int k = 0 ; k < rx_rtuPduLength ; k++)
            {
                printf(" %X", ModbusASCII_tempBuffer[ k ] );
            } 
            printf("\n");
            #endif 

            rx_pduLRC = calculateLRC( ModbusASCII_tempBuffer, ( rx_rtuPduLength - 1 ) );

            if( rx_pduLRC != ModbusASCII_tempBuffer[ rx_rtuPduLength -1 ] )
            {
                retValueTemp = MODBUS_FRAME_CHECK_ERROR;
                return retValueTemp;
            }

            if( ModbusASCII_tempBuffer[ 0 ] != deviceIDParam )
            {
                // id mismatch..
                retValueTemp = MODBUS_WRONG_ID_ERROR;
                return retValueTemp;
            }
            
            if( ( ModbusASCII_tempBuffer[1] > 0x80 ) && ( ( ModbusASCII_tempBuffer[1] < 0x91 ) ) )
            {
                //modbus error
                printf("modbusError Code : %X, %X \n", ModbusASCII_tempBuffer[ 1 ], ( ModbusASCII_tempBuffer[ 1 ] - 0x80 ) );
                return MODBUS_RESPONSE_TIMEOUT;
            }

            if ( ModbusASCII_tempBuffer[ 1 ] != WRITE_MULTI_HOLDING_REGISTERS_FCODE )
            {
                // fcode mismatch mq
                retValueTemp = MODBUS_WRONG_FCODE_ERROR;
                return retValueTemp;
            }
        }
        else
        {

        }
    }
    else
    {
        retValueTemp = MODBUS_RESPONSE_TIMEOUT;
        return retValueTemp;
    }

    return retValueTemp;
}

uint8_t   calculateLRC( uint8_t *pduPayloadParam, uint16_t lengthParam )
{
    uint8_t sumTemp = 0;
    uint8_t twosComplementOfSumTemp = 0;
    for( int k = 0; k < lengthParam; k++ )
    {
        sumTemp = ( sumTemp + pduPayloadParam[ k ] ) & 0xFF; // throw away any bits that carry over 8 bits.
    }
    twosComplementOfSumTemp = (-(unsigned int)sumTemp);
    return twosComplementOfSumTemp;
}

bool convertModbusFramefromRTUtoASCII( uint8_t *inputPduParam, uint16_t inputPduLenghtParam, uint8_t *outputPduParam, uint16_t *outputPduLengthParam )
{
    uint16_t asciiBufferLength = ( ( inputPduLenghtParam * 2 ) + 1 );
    uint8_t asciiBuffer[ asciiBufferLength ];
    char    twoNibbleStrTemp[ 3 ] = { '0','0', 0 };
    uint8_t lowNibbleTemp = 0;
    uint8_t highNibbleTemp = 0;

    asciiBuffer[ 0 ] = 0x3A; // first byte always gonna be 0x3A;

    for( int i = 0 ; i < inputPduLenghtParam ; i ++ )
    {
        lowNibbleTemp = ( inputPduParam[ i ] & 0x0F );
        highNibbleTemp = ( inputPduParam[ i ] >> 4 );
        sprintf( twoNibbleStrTemp, "%X%X",  highNibbleTemp, lowNibbleTemp );
        //printf("byte : %d , nibbles : %d, %d, twoNibbleStrTemp : %s\n", pduBufferParam[ i ], lowNibbleTemp, highNibbleTemp, twoNibbleStrTemp );
        asciiBuffer[ 1 + (2 * i) ] = twoNibbleStrTemp[ 0 ];
        asciiBuffer[ 2 + (2 * i) ] = twoNibbleStrTemp[ 1 ];
    }
    memcpy( outputPduParam, asciiBuffer, asciiBufferLength );
    *( outputPduLengthParam ) = asciiBufferLength;

    #if 0
    printf("<ModbusASCII Frame : ");
    for ( int i = 0 ; i < ( inputPduLenghtParam * 2 ) + 1 ; i ++ )
    {
        printf("%x ", asciiBuffer[i] );
    }
    printf("\n");
    #endif

    return true;
}

bool convertModbusFramefromASCIItoRTU( uint8_t *inputPduParam, uint16_t inputPduLenghtParam, uint8_t *outputPduParam, uint16_t *outputPduLengthParam )
{
    /* parameter check...*/
    if( ( inputPduLenghtParam % 2 ) != 1 )
    {
        printf( "<convertModbusPDUfromASCIItoRTU> pduLength hatali...\n");
        return false;
    }

    if( inputPduParam[ 0 ] != 0x3A )
    {
        printf( "<convertModbusPDUfromASCIItoRTU> ASCII uygun PDU degil. ilk Byte Hatali  -> %d\n", inputPduParam[ 0 ] );
        return false;
    }
    if( /* ( inputPduParam[ inputPduLenghtParam - 2 ] != 0x0D ) || */ ( inputPduParam[ inputPduLenghtParam - 1 ] != 0x0A ) )
    {
        printf( "<convertModbusPDUfromASCIItoRTU> ASCII uygun PDU degil. Son iki byte Hatali  -> %d, %d\n", inputPduParam[ inputPduLenghtParam - 2 ], inputPduParam[ inputPduLenghtParam - 1 ] );
        return false;
    }

    uint16_t    rtuPduLength = ( ( inputPduLenghtParam - 1 ) / 2 ) - 1; // discard last CR and LF bytes...
    uint8_t     rtuBuffer[ rtuPduLength ];

    char        twoNibbleStrTemp[ 3 ] = { '0','0', 0 };
    int         byteValueTemp = 0x00;

    for( int i = 0 ; i < rtuPduLength ; i ++ )
    {
        twoNibbleStrTemp[ 0 ] = inputPduParam[ 1 + ( 2 * i ) ];
        twoNibbleStrTemp[ 1 ] = inputPduParam[ 2 + ( 2 * i ) ];
        sscanf( twoNibbleStrTemp, "%X", &byteValueTemp );
        rtuBuffer[ i ] = byteValueTemp;
    }
    #if 0
    printf("<ModbusRTU Frame : ");

    for( int i = 0 ; i < rtuPduLength ; i++ )
    {
        printf("%d ", rtuBuffer[i]);
    }
    printf("\n");
    #endif
    memcpy( outputPduParam, rtuBuffer, rtuPduLength );
    *( outputPduLengthParam ) = rtuPduLength;
    return true;
}


bool checkForValidNibbleValue( uint8_t nibbleParam )
{
    if ( (0x00 <= nibbleParam ) && ( 0x0F >= nibbleParam ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


