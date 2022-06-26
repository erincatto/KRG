#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_ENGINETOOLS
        #define KRG_ENGINETOOLS_API __declspec(dllexport)
    #else
        #define KRG_ENGINETOOLS_API __declspec(dllimport)
    #endif
#else
    #define KRG_ENGINETOOLS_API
#endif