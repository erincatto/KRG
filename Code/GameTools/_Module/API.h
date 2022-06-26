#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_GAMETOOLS
        #define KRG_GAMETOOLS_API __declspec(dllexport)
    #else
        #define KRG_GAMETOOLS_API __declspec(dllimport)
    #endif
#else
    #define KRG_GAMETOOLS_API
#endif