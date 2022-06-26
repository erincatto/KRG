#pragma once
//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_GAME
        #define KRG_GAME_API __declspec(dllexport)
    #else
        #define KRG_GAME_API __declspec(dllimport)
    #endif
#else
    #define KRG_GAME_API
#endif