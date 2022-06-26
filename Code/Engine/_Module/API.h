#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_ENGINE
        #define KRG_ENGINE_API __declspec(dllexport)
    #else
        #define KRG_ENGINE_API __declspec(dllimport)
    #endif
#else
    #define KRG_ENGINE_API
#endif