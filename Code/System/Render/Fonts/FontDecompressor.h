#pragma once

#include "../_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/KRG.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Fonts
    {
        KRG_SYSTEM_RENDER_API void GetDecompressedFontData( uint8_t const* pSourceData, TVector<uint8_t>& fontData );
    }
}