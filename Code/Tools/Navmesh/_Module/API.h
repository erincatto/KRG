#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_TOOLS_NAVMESH
        #define KRG_TOOLS_NAVMESH_API __declspec(dllexport)
    #else
        #define KRG_TOOLS_NAVMESH_API __declspec(dllimport)
    #endif
#else
    #define KRG_TOOLS_NAVMESH_API
#endif