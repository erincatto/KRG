#pragma once

#include "System/_Module/API.h"
#include "System/Math/Math.h"
#include "System/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG { class IniFile; }

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class KRG_SYSTEM_API ResourceSettings
    {
    public:

        bool ReadSettings( IniFile const& ini );

    public:

        FileSystem::Path        m_workingDirectoryPath;
        FileSystem::Path        m_compiledResourcePath;

        #if KRG_DEVELOPMENT_TOOLS
        FileSystem::Path        m_packagedBuildCompiledResourcePath;
        String                  m_resourceServerNetworkAddress;
        uint16_t                m_resourceServerPort;
        FileSystem::Path        m_rawResourcePath;
        FileSystem::Path        m_compiledResourceDatabasePath;
        FileSystem::Path        m_resourceServerExecutablePath;
        FileSystem::Path        m_resourceCompilerExecutablePath;
        #endif
    };
}