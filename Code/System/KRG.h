#pragma once

#include <stdint.h>

//-------------------------------------------------------------------------

using nullptr_t = decltype( nullptr );

//-------------------------------------------------------------------------
// Common
//-------------------------------------------------------------------------

#define KRG_STRINGIZING(x) #x
#define KRG_MAKE_STRING(x) KRG_STRINGIZING(x)
#define KRG_FILE_LINE __FILE__ ":" KRG_MAKE_STRING(__LINE__)

//-------------------------------------------------------------------------
// Configurations
//-------------------------------------------------------------------------

// KRG_DEBUG = unoptimized build with debug info (debug drawing, string debug names, etc...)
// KRG_RELEASE = optimized build with debug info (debug drawing, string debug names, etc...)
// KRG_SHIPPING = optimized build with no debug info

//-------------------------------------------------------------------------
// Development Tools
//-------------------------------------------------------------------------
// KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO exists for use in macros.

#if !KRG_SHIPPING
#define KRG_DEVELOPMENT_TOOLS 1
#endif

#if KRG_DEVELOPMENT_TOOLS
#define KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( x ) x
#define KRG_DEVELOPMENT_TOOLS_ONLY( x ) x
#else
#define KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( x )
#define KRG_DEVELOPMENT_TOOLS_ONLY( x )
#endif

//-------------------------------------------------------------------------
// Platform specific defines
//-------------------------------------------------------------------------

#if _WIN32
#include "Platform/Platform_Win32.h"
#endif

//-------------------------------------------------------------------------
// Debug defines
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS

    // Platform agnostic asserts
    //-------------------------------------------------------------------------

    #define KRG_STATIC_ASSERT( cond, error ) static_assert( cond, error )
    #define KRG_TRACE_ASSERT( msg ) { KRG_TRACE_MSG( msg ); KRG_HALT(); }
    #define KRG_UNIMPLEMENTED_FUNCTION() KRG_TRACE_ASSERT("Function not implemented!\n")
    #define KRG_UNREACHABLE_CODE() KRG_TRACE_ASSERT("Unreachable code encountered!\n")

#else

    // Platform specific, need to be defined in Platform/Defines_XXX.h

    #define KRG_ASSERT( cond )
    #define KRG_TRACE_MSG_WIN32( msg )
    #define KRG_BREAK()
    #define KRG_HALT()

    #define KRG_DISABLE_OPTIMIZATION
    #define KRG_ENABLE_OPTIMIZATION

    // Platform agnostic asserts
    //-------------------------------------------------------------------------

    #define KRG_STATIC_ASSERT( cond, error )
    #define KRG_TRACE_MSG( msg ) 
    #define KRG_TRACE_ASSERT( msg )
    #define KRG_UNIMPLEMENTED_FUNCTION()
    #define KRG_UNREACHABLE_CODE()
#endif

//-------------------------------------------------------------------------
// Core typedefs
//-------------------------------------------------------------------------

namespace KRG
{
    #define InvalidIndex -1

    //-------------------------------------------------------------------------

    namespace Platform
    {
        enum class Target
        {
            PC = 0,
        };
    }
}