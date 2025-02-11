#pragma once
#include "System/Types/Arrays.h"

//-------------------------------------------------------------------------

namespace KRG::Encoding
{
    //-------------------------------------------------------------------------
    // Base 64 Encoding
    //-------------------------------------------------------------------------

    namespace Base64
    {
        KRG_SYSTEM_API Blob Encode( uint8_t const* pDataToEncode, size_t dataSize );
        KRG_SYSTEM_API Blob Decode( uint8_t const* pDataToDecode, size_t dataSize );
    }

    //-------------------------------------------------------------------------
    // Base 85 Encoding
    //-------------------------------------------------------------------------

    namespace Base85
    {
        KRG_SYSTEM_API Blob Encode( uint8_t const* pDataToEncode, size_t dataSize );
        KRG_SYSTEM_API Blob Decode( uint8_t const* pDataToDecode, size_t dataSize );
    }
}