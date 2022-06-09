#include "ResourceCompiler_Navmesh.h"
#include "Tools/Navmesh/NavmeshGenerator.h"
#include "Engine/Navmesh/NavmeshData.h"
#include "Engine/Core/Entity/EntitySerialization.h"
#include "Engine/Core/Entity/EntityDescriptors.h"
#include "Engine/Navmesh/Components/Component_Navmesh.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Time/Timers.h"
#include "System/TypeSystem/TypeRegistry.h"
#include <filesystem>

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    NavmeshCompiler::NavmeshCompiler()
        : Resource::Compiler( "NavmeshCompiler", NavmeshGenerator::s_version )
    {
        m_outputTypes.push_back( NavmeshData::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult NavmeshCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        if ( ctx.m_inputFilePath.Exists() )
        {
            std::error_code ec;
            std::filesystem::copy_options opts = std::filesystem::copy_options::overwrite_existing;
            std::filesystem::copy( ctx.m_inputFilePath.c_str(), ctx.m_outputFilePath.c_str(), opts, ec );
            return ( ec.value() == 0 ) ? CompilationSucceeded(ctx) : CompilationFailed(ctx);
        }
        else
        {
            return GenerateNavmesh( ctx );
        }
    }

    Resource::CompilationResult NavmeshCompiler::GenerateNavmesh( Resource::CompileContext const& ctx ) const
    {
        Message( "No pre-generated navmesh found, generating navmesh!" );

        // Get map data
        //-------------------------------------------------------------------------

        FileSystem::Path mapPath = ctx.m_inputFilePath;
        mapPath.ReplaceExtension( "map" );

        if ( !mapPath.Exists() )
        {
            Error( "Entity map file (%s) doesnt exist!", mapPath.c_str() );
            return Resource::CompilationResult::Failure;
        }

        EntityModel::EntityMapDescriptor mapDesc;

        Milliseconds elapsedTime = 0.0f;
        {
            ScopedTimer<PlatformClock> timer( elapsedTime );

            if ( !EntityModel::Serializer::ReadEntityCollectionFromFile( *m_pTypeRegistry, mapPath, mapDesc ) )
            {
                Error( "Entity map file (%s) is malformed!", mapPath.c_str() );
                return Resource::CompilationResult::Failure;
            }
        }

        Message( "Entity map read in: %.2fms", elapsedTime.ToFloat() );

        // Get navmesh component
        //-------------------------------------------------------------------------

        bool hasWarning = false;
        auto const navmeshComponents = mapDesc.GetComponentsOfType<NavmeshComponent>( *m_pTypeRegistry, false );
        if ( navmeshComponents.empty() )
        {
            Error( "Requesting navmesh for a map without a navmesh component! This is an invalid operation!" );
            return CompilationFailed( ctx );
        }


        if ( navmeshComponents.size() > 1 )
        {
            Warning( "More than one navmesh component found in this map, this is not supported... Ignoring all components apart from the first found!" );
        }

        auto pNavmeshComponent = navmeshComponents[0].m_pComponent->CreateTypeInstance<Navmesh::NavmeshComponent>( *m_pTypeRegistry );
        KRG_ASSERT( pNavmeshComponent != nullptr );
        Navmesh::NavmeshBuildSettings const buildSettings = pNavmeshComponent->GetBuildSettings();
        KRG::Delete( pNavmeshComponent );

        // Generate navmesh
        //-------------------------------------------------------------------------

        Navmesh::NavmeshGenerator generator( *m_pTypeRegistry, m_rawResourceDirectoryPath, ctx.m_outputFilePath, mapDesc, buildSettings );

        {
            ScopedTimer<PlatformClock> timer( elapsedTime );
            generator.GenerateSync();
        }

        Message( "Navmesh built in: %.2fms", elapsedTime.ToFloat() );

        //-------------------------------------------------------------------------

        return hasWarning ? Resource::CompilationResult::SuccessWithWarnings : Resource::CompilationResult::Success;
    }
}