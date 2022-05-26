#include "_AutoGenerated/ToolsTypeRegistration.h"
#include "Tools/Animation/_Module/Module.h"
#include "Tools/Render/_Module/Module.h"
#include "Tools/Physics/_Module/Module.h"
#include "Tools/Entity/_Module/Module.h"
#include "Applications/Shared/ApplicationGlobalState.h"
#include "Applications/Shared/cmdParser/krg_cmdparser.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/FileSystem/FileSystemUtils.h"
#include "System/Core/Logging/Log.h"

#include <windows.h>
#include <iostream>

//-------------------------------------------------------------------------

using namespace KRG;

//-------------------------------------------------------------------------
// Command Line Argument Parsing
//-------------------------------------------------------------------------

namespace KRG
{
    struct CommandLineArgumentParser
    {
        CommandLineArgumentParser( int argc, char* argv[] )
        {
            cli::Parser cmdParser( argc, argv );
            cmdParser.set_default<bool>( false );
            cmdParser.set_optional<std::string>( "compile", "compile", "", "Compile resource" );
            cmdParser.set_optional<bool>( "debug", "debug", false, "Trigger debug break before execution." );

            if ( cmdParser.run() )
            {
                m_triggerDebugBreak = cmdParser.get<bool>( "debug" );

                // Get compile argument
                ResourcePath const resourcePath( cmdParser.get<std::string>( "compile" ).c_str() );
                if ( resourcePath.IsValid() )
                {
                    m_resourceID = ResourceID( resourcePath );

                    if ( m_resourceID.IsValid() )
                    {
                        m_isValid = true;
                    }
                    else
                    {
                        KRG_LOG_ERROR( "ResourceCompiler", "Invalid compile request: %s\n", m_resourceID.ToString().c_str() );
                    }

                    return;
                }
            }
        }

        bool IsValid() const { return m_isValid; }

    public:

        ResourceID          m_resourceID;
        bool                m_triggerDebugBreak = false;
        bool                m_isValid = false;
    };
}

static int32 CompileResource( TypeSystem::TypeRegistry const& typeRegistry, Resource::CompilerRegistry const& compilerRegistry, Resource::Settings const& settings, ResourceID const& resourceID )
{
    
}

//-------------------------------------------------------------------------
// Application Entry Point
//-------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    ApplicationGlobalState State;

    // Read INI settings
    //-------------------------------------------------------------------------

    FileSystem::Path const iniPath = FileSystem::GetCurrentProcessPath() + "KRG.ini";

    Resource::Settings settings;
    SettingsRegistry settingsRegistry;
    settingsRegistry.RegisterSettings( &settings );

    if ( !settingsRegistry.LoadFromFile( iniPath ) )
    {
        KRG_LOG_ERROR( "ResourceCompiler", "Failed to read settings from INI file: %s", iniPath.c_str() );
        return 1;
    }

    // Read CMD line arguments and process request
    //-------------------------------------------------------------------------

    CommandLineArgumentParser argParser( argc, argv );

    for ( int i = 0; i < argc; i++ )
    {
        std::cout << argv[i] << std::endl;
    }

    if ( !argParser.IsValid() )
    {
        KRG_LOG_ERROR( "ResourceCompiler", "Invalid command line arguments" );
        return 1;
    }

    // File Paths
    //-------------------------------------------------------------------------

    settings.m_rawResourcePath.EnsureDirectoryExists();
    settings.m_compiledResourcePath.EnsureDirectoryExists();

    // Create tools modules and register compilers
    //-------------------------------------------------------------------------

    TypeSystem::TypeRegistry typeRegistry;
    Resource::CompilerRegistry compilerRegistry;
    Animation::ToolsModule animationModule;
    Render::ToolsModule renderModule;
    Physics::ToolsModule physicsModule;
    EntityModel::ToolsModule entityModule;

    AutoGenerated::Tools::RegisterTypes( typeRegistry );

    animationModule.RegisterCompilers( compilerRegistry );
    renderModule.RegisterCompilers( compilerRegistry );
    physicsModule.RegisterCompilers( compilerRegistry );
    entityModule.RegisterCompilers( compilerRegistry );

    // Execute compilation command
    //-------------------------------------------------------------------------

    // Debug break
    if ( argParser.m_triggerDebugBreak )
    {
        KRG_HALT();
    }

    auto CompileResource = [&] ()
    {
        // Try create compilation context
        Resource::CompileContext compileContext( typeRegistry, settings.m_rawResourcePath, settings.m_compiledResourcePath, argParser.m_resourceID );
        if ( !compileContext.IsValid() )
        {
            return -1;
        }

        // Validate input path
        if ( !FileSystem::Exists( compileContext.m_inputFilePath ) )
        {
            KRG_LOG_ERROR( "ResourceCompiler", "Source file for data path ('%s') does not exist: '%s'\n", settings.m_rawResourcePath.c_str(), compileContext.m_inputFilePath.c_str() );
            return -1;
        }

        // Try find compiler
        auto pCompiler = compilerRegistry.GetCompilerForResourceType( compileContext.m_resourceID.GetResourceTypeID() );
        if ( pCompiler == nullptr )
        {
            KRG_LOG_ERROR( "ResourceCompiler", "Cant find appropriate resource compiler for type: %u", compileContext.m_resourceID.GetResourceTypeID() );
            return -1;
        }

        // Compile
        Resource::CompilationResult const result = pCompiler->Compile( compileContext );
        return (int32) result;
    };

    int32 const result = CompileResource();

    // Unregister all compilers and modules
    //-------------------------------------------------------------------------

    animationModule.UnregisterCompilers( compilerRegistry );
    renderModule.UnregisterCompilers( compilerRegistry );
    physicsModule.UnregisterCompilers( compilerRegistry );
    entityModule.UnregisterCompilers( compilerRegistry );

    AutoGenerated::Tools::UnregisterTypes( typeRegistry );

    return result;
}