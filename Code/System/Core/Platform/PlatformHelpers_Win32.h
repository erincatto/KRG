#pragma once
#ifdef _WIN32

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------
// Platform Specific Helpers/Functions
//-------------------------------------------------------------------------

namespace KRG::Platform::Win32
{
    // File system
    //-------------------------------------------------------------------------

    KRG_SYSTEM_CORE_API String GetShortPath( String const& origPath );
    KRG_SYSTEM_CORE_API String GetLongPath( String const& origPath );

    // Processes
    //-------------------------------------------------------------------------

    KRG_SYSTEM_CORE_API uint32_t GetProcessID( char const* processName );
    KRG_SYSTEM_CORE_API String GetProcessPath( uint32_t processID );
    KRG_SYSTEM_CORE_API String GetCurrentModulePath();
    KRG_SYSTEM_CORE_API String GetLastErrorMessage();

    // Try to start a window process and returns the process ID
    KRG_SYSTEM_CORE_API uint32_t StartProcess( char const* exePath, char const* cmdLine = nullptr );

    // Kill a running process
    KRG_SYSTEM_CORE_API bool KillProcess( uint32_t processID );

    // Check if a named process is currently running
    inline bool IsProcessRunning( char const* processName, uint32_t* pProcessID ) { return GetProcessID( processName ) != 0; }

    // Open a path in explorer
    KRG_SYSTEM_CORE_API void OpenInExplorer( char const* path );
}
#endif