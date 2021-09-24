#pragma once

#include "System/Core/Types/IntegralTypes.h"

//-------------------------------------------------------------------------

#ifdef _WIN32

    typedef unsigned char BYTE;

    #ifdef KRG_DEBUG
        #include "_AutoGenerated/PS_Textured_x64_Debug.h"
        #include "_AutoGenerated/VS_SkinnedPrimitive_x64_Debug.h"
        #include "_AutoGenerated/VS_StaticPrimitive_x64_Debug.h"
    #else
        #include "_AutoGenerated/PS_Textured_x64_Release.h"
        #include "_AutoGenerated/VS_SkinnedPrimitive_x64_Release.h"
        #include "_AutoGenerated/VS_StaticPrimitive_x64_Release.h"
    #endif
#endif