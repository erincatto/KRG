#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #if KRG_SYSTEM
        #define KRG_SYSTEM_API __declspec(dllexport)
    #else
        #define KRG_SYSTEM_API __declspec(dllimport)
    #endif
#else
    #define KRG_SYSTEM_API
#endif