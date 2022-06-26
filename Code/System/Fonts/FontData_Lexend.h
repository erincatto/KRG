#pragma once
#include "../_Module/API.h"
#include "System/KRG.h"

//-------------------------------------------------------------------------

namespace KRG::Fonts::Lexend
{
    namespace Regular
    {
        KRG_SYSTEM_API uint8_t const* GetData();
    }

    namespace Medium
    {
        KRG_SYSTEM_API uint8_t const* GetData();
    }

    namespace Bold
    {
        KRG_SYSTEM_API uint8_t const* GetData();
    }
};