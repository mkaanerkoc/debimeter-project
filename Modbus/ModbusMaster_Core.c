#include "ModbusMaster_Core.h"

uint8_t modbusCoreBuffer[ 256 ];

void ModbusMaster_PrepareWriteHoldingRegisterPDU( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t valueParam , uint8_t *pduParam , uint16_t *pduLengthParam )
{
    uint8_t messageSizeLength = 0;
    modbusCoreBuffer[ messageSizeLength++ ] = deviceIDParam;
    modbusCoreBuffer[ messageSizeLength++ ] = WRITE_HOLDING_REGISTER_FCODE;
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam & 0xFF );
    modbusCoreBuffer[ messageSizeLength++ ] = ( valueParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( valueParam & 0xFF );
    memcpy( pduParam, modbusCoreBuffer, messageSizeLength );
    ( *pduLengthParam ) = messageSizeLength;
}

void ModbusMaster_PrepareWriteMultiHoldingRegistersPDU( uint8_t deviceIDParam, uint16_t registerAddressParam, uint16_t *valuesArrayParam , uint8_t registerCountParam , uint8_t *pduParam  , uint16_t *pduLengthParam )
{
    uint8_t messageSizeLength = 0;
    modbusCoreBuffer[ messageSizeLength++ ] = deviceIDParam;
    modbusCoreBuffer[ messageSizeLength++ ] = WRITE_MULTI_HOLDING_REGISTERS_FCODE;
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam & 0xFF );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerCountParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerCountParam & 0xFF );
    //04: The number of data bytes to follow (2 registers x 2 bytes each = 4 bytes) -> unuttuk...
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerCountParam * 2 );
    for( int k = 0; k < registerCountParam ; k++ )
    {
        modbusCoreBuffer[ messageSizeLength++ ] = ( uint8_t )( valuesArrayParam[ k ] >> 8 );
        modbusCoreBuffer[ messageSizeLength++ ] = ( uint8_t )( valuesArrayParam[ k ] & 0xFF );
    }
    memcpy( pduParam, modbusCoreBuffer, messageSizeLength );
    ( *pduLengthParam ) = messageSizeLength;
}


void ModbusMaster_PrepareReadHoldingRegistersPDU( uint8_t deviceIDParam, uint16_t registerAddressParam , uint16_t registerCountParam , uint8_t *pduParam , uint16_t *pduLengthParam  )
{
    uint8_t messageSizeLength = 0;
    modbusCoreBuffer[ messageSizeLength++ ] = deviceIDParam;
    modbusCoreBuffer[ messageSizeLength++ ] = READ_HOLDING_REGISTERS_FCODE;
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam & 0xFF );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerCountParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerCountParam & 0xFF );
    memcpy( pduParam, modbusCoreBuffer, messageSizeLength );
    ( *pduLengthParam ) = messageSizeLength;
}

void ModbusMaster_PrepareReadInputRegistersPDU( uint8_t deviceIDParam, uint16_t registerAddressParam,  uint16_t registerCountParam , uint8_t *pduParam , uint16_t *pduLengthParam  )
{
    uint8_t messageSizeLength = 0;
    modbusCoreBuffer[ messageSizeLength++ ] = deviceIDParam;
    modbusCoreBuffer[ messageSizeLength++ ] = READ_INPUT_REGISTERS_FCODE;
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerAddressParam & 0xFF );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerCountParam >> 8 );
    modbusCoreBuffer[ messageSizeLength++ ] = ( registerCountParam & 0xFF );
    memcpy( pduParam, modbusCoreBuffer, messageSizeLength );
    ( *pduLengthParam ) = messageSizeLength;
}
