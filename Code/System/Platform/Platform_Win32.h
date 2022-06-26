#pragma once
#ifdef _WIN32

#include "System/_Module/API.h"

//-------------------------------------------------------------------------

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define KRG_FORCE_INLINE __forceinline

//-------------------------------------------------------------------------
// Enable specific warnings
//-------------------------------------------------------------------------

#pragma warning(default:4800)
#pragma warning(default:4389)

//-------------------------------------------------------------------------
// Core Functions
//-------------------------------------------------------------------------

namespace KRG::Platform::Win32
{
    // Prints a message to the output log with a newline
    KRG_SYSTEM_API void OutputDebugMessage( char const* msg );
}

//-------------------------------------------------------------------------
// Debug defines
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS

#define KRG_DISABLE_OPTIMIZATION __pragma( optimize( "", off ) )
#define KRG_ENABLE_OPTIMIZATION __pragma( optimize( "", on ) )

#define KRG_TRACE_MSG( msg ) KRG::Platform::Win32::OutputDebugMessage( msg )
#define KRG_ASSERT( cond ) do { if( !(cond) ) { KRG_TRACE_MSG( "KRG Assert fired: " #cond " (" KRG_FILE_LINE ")"  ); __debugbreak(); } } while( 0 )
#define KRG_BREAK() __debugbreak()
#define KRG_HALT() __debugbreak()

#endif

#endif