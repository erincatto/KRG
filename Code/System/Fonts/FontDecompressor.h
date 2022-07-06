#pragma once

#include "../_Module/API.h"
#include "System/Types/Arrays.h"
#include "System/KRG.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Fonts
    {
        KRG_SYSTEM_API void GetDecompressedFontData( uint8_t const* pSourceData, Blob& fontData );
    }
}