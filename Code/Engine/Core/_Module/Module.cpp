#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Resource/ResourceProviders/NetworkResourceProvider.h"
#include "System/Resource/ResourceProviders/PackagedResourceProvider.h"
#include "System/Network/NetworkSystem.h"

//-------------------------------------------------------------------------

#ifdef _WIN32
#include "System/Core/Platform/PlatformHelpers_Win32.h"
#endif

//-------------------------------------------------------------------------

namespace KRG
{
    #if KRG_DEVELOPMENT_TOOLS
    bool EnsureResourceServerIsRunning( FileSystem::Path const& resourceServerExecutablePath )
    {
        #if _WIN32
        bool shouldStartResourceServer = false;

        // If the resource server is not running then start it
        String const resourceServerExecutableName = resourceServerExecutablePath.GetFilename();
        uint32_t const resourceServerProcessID = Platform::Win32::GetProcessID( resourceServerExecutableName.c_str() );
        shouldStartResourceServer = ( resourceServerProcessID == 0 );

        // Ensure we are running the correct build of the resource server
        if ( !shouldStartResourceServer )
        {
            String const resourceServerPath = Platform::Win32::GetProcessPath( resourceServerProcessID );
            if ( !resourceServerPath.empty() )
            {
                FileSystem::Path const resourceServerProcessPath = FileSystem::Path( resourceServerPath ).GetParentDirectory();
                FileSystem::Path const applicationDirectoryPath = FileSystem::Path( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();

                if ( resourceServerProcessPath != applicationDirectoryPath )
                {
                    Platform::Win32::KillProcess( resourceServerProcessID );
                    shouldStartResourceServer = true;
                }
            }
            else
            {
                return false;
            }
        }

        // Try to start the resource server
        if ( shouldStartResourceServer )
        {
            FileSystem::Path const applicationDirectoryPath = FileSystem::Path( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();
            return Platform::Win32::StartProcess( resourceServerExecutablePath.c_str() ) != 0;
        }

        return true;
        #else
        return false;
        #endif
    }
    #endif
}

//-------------------------------------------------------------------------

namespace KRG::EngineCore
{
    bool EngineModule::Initialize( ModuleContext& context, IniFile const& iniFile )
    {
        if ( !Network::NetworkSystem::Initialize() )
        {
            KRG_LOG_ERROR( "Render", "Failed to initialize network system" );
            return false;
        }

        // Create and initialize resource provider
        //-------------------------------------------------------------------------

        Resource::ResourceSettings settings;
        if ( !settings.ReadSettings( iniFile ) )
        {
            KRG_LOG_ERROR( "Resource Provider", "Failed to read resource settings from ini file!" );
            return false;
        }

        #if KRG_DEVELOPMENT_TOOLS
        {
            if ( !EnsureResourceServerIsRunning( settings.m_resourceServerExecutablePath ) )
            {
                KRG_LOG_ERROR( "Resource Provider", "Couldn't start resource server (%s)!", settings.m_resourceServerExecutablePath.c_str() );
                return false;
            }

            m_pResourceProvider = KRG::New<Resource::NetworkResourceProvider>( settings );
        }
        #else
        {
            m_pResourceProvider = KRG::New<Resource::PackagedResourceProvider>( settings );
        }
        #endif

        if ( m_pResourceProvider == nullptr )
        {
            KRG_LOG_ERROR( "Resource", "Failed to create resource provider" );
            return false;
        }

        if ( !m_pResourceProvider->Initialize() )
        {
            KRG_LOG_ERROR( "Resource", "Failed to intialize resource provider" );
            KRG::Delete( m_pResourceProvider );
            return false;
        }

        // Create and initialize render device
        //-------------------------------------------------------------------------

        m_pRenderDevice = KRG::New<Render::RenderDevice>();
        if ( !m_pRenderDevice->Initialize( iniFile ) )
        {
            KRG_LOG_ERROR( "Render", "Failed to create render device" );
            KRG::Delete( m_pRenderDevice );
            return false;
        }

        // Initialization
        //-------------------------------------------------------------------------

        m_taskSystem.Initialize();
        m_resourceSystem.Initialize( m_pResourceProvider );
        m_inputSystem.Initialize();

        #if KRG_DEVELOPMENT_TOOLS
        m_imguiSystem.Initialize( context.GetApplicationName() + ".imgui.ini", m_pRenderDevice, m_imguiViewportsEnabled );
        #endif

        // Register systems
        //-------------------------------------------------------------------------

        m_systemRegistry.RegisterSystem( &m_settingsRegistry );
        m_systemRegistry.RegisterSystem( &m_typeRegistry );
        m_systemRegistry.RegisterSystem( &m_taskSystem );
        m_systemRegistry.RegisterSystem( &m_resourceSystem );
        m_systemRegistry.RegisterSystem( &m_inputSystem );
        m_systemRegistry.RegisterSystem( &m_entityWorldManager );

        // Register resource loaders
        //-------------------------------------------------------------------------

        m_entityCollectionLoader.SetTypeRegistry( &m_typeRegistry );
        m_resourceSystem.RegisterResourceLoader( &m_entityCollectionLoader );

        //-------------------------------------------------------------------------

        return true;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        bool wasFullyInitialized = m_pResourceProvider != nullptr && m_pRenderDevice != nullptr;

        //-------------------------------------------------------------------------

        if ( wasFullyInitialized )
        {
            // Unregister resource loaders
            //-------------------------------------------------------------------------

            m_resourceSystem.UnregisterResourceLoader( &m_entityCollectionLoader );

            // Unregister systems
            //-------------------------------------------------------------------------

            m_systemRegistry.UnregisterSystem( &m_entityWorldManager );
            m_systemRegistry.UnregisterSystem( &m_inputSystem );
            m_systemRegistry.UnregisterSystem( &m_resourceSystem );
            m_systemRegistry.UnregisterSystem( &m_taskSystem );
            m_systemRegistry.UnregisterSystem( &m_typeRegistry );
            m_systemRegistry.UnregisterSystem( &m_settingsRegistry );

            // Shutdown
            //-------------------------------------------------------------------------

            #if KRG_DEVELOPMENT_TOOLS
            m_imguiSystem.Shutdown();
            #endif

            m_inputSystem.Shutdown();
            m_resourceSystem.Shutdown();
            m_taskSystem.Shutdown();
        }

        //-------------------------------------------------------------------------

        if ( m_pRenderDevice != nullptr )
        {
            m_pRenderDevice->Shutdown();
            KRG::Delete( m_pRenderDevice );
        }

        if ( m_pResourceProvider != nullptr )
        {
            m_pResourceProvider->Shutdown();
            KRG::Delete( m_pResourceProvider );
        }

        Network::NetworkSystem::Shutdown();
    }
}