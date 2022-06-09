#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_TOOLS_AI
        #define KRG_TOOLS_AI_API __declspec(dllexport)
    #else
        #define KRG_TOOLS_AI_API __declspec(dllimport)
    #endif
#else
    #define KRG_TOOLS_AI_API
#endif