#include "ResourceCompiler_PhysicsMaterialDatabase.h"
#include "EngineTools/Physics/ResourceDescriptors/ResourceDescriptor_PhysicsMaterialDatabase.h"
#include "System/FileSystem/FileSystem.h"
#include "System/Serialization/BinarySerialization.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    PhysicsMaterialDatabaseCompiler::PhysicsMaterialDatabaseCompiler()
        : Resource::Compiler( "PhysicsMaterialCompiler", s_version )
    {
        m_outputTypes.push_back( PhysicsMaterialDatabase::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult PhysicsMaterialDatabaseCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        PhysicsMaterialDatabaseResourceDescriptor resourceDescriptor;
        if ( !Resource::ResourceDescriptor::TryReadFromFile( *m_pTypeRegistry, ctx.m_inputFilePath, resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Read material libraries
        //-------------------------------------------------------------------------

        TVector<PhysicsMaterialSettings> materialSettings;

        for ( ResourcePath const& libraryPath : resourceDescriptor.m_materialLibraries )
        {
            FileSystem::Path libraryFilePath;
            if ( !ConvertResourcePathToFilePath( libraryPath, libraryFilePath ) )
            {
                return Error( "Failed to convert data path to filepath: %s", libraryPath.c_str() );
            }

            //-------------------------------------------------------------------------

            Serialization::TypeArchiveReader typeReader( *m_pTypeRegistry );
            if ( typeReader.ReadFromFile( libraryFilePath ) )
            {
                int32_t const numSerializedTypes = typeReader.GetNumSerializedTypes();
                if ( numSerializedTypes == 0 )
                {
                    KRG_LOG_ERROR( "ResourceCompiler", "Empty physics material library encountered: %s", libraryFilePath.c_str() );
                    return CompilationFailed( ctx );
                }

                for ( auto i = 0; i < numSerializedTypes; i++ )
                {
                    PhysicsMaterialSettings material;
                    typeReader >> material;

                    // Validity check
                    if ( !material.IsValid() )
                    {
                        KRG_LOG_ERROR( "ResourceCompiler", "Invalid physics material encountered in library: %s", libraryFilePath.c_str() );
                        return CompilationFailed( ctx );
                    }

                    // Duplicate check
                    auto DuplicateCheck = [] ( PhysicsMaterialSettings const& materialSettings, StringID const& ID )
                    {
                        return materialSettings.m_ID == ID;
                    };

                    if ( eastl::find( materialSettings.begin(), materialSettings.end(), material.m_ID, DuplicateCheck ) != materialSettings.end() )
                    {
                        KRG_LOG_ERROR( "ResourceCompiler", "Duplicate physics material ID '%s' detected", material.m_ID.c_str() );
                        return CompilationFailed( ctx );
                    }

                    // Add valid material
                    materialSettings.emplace_back( material );
                }
            }
            else
            {
                KRG_LOG_ERROR( "ResourceCompiler", "Failed to read physics material library file: %s", libraryFilePath.c_str() );
                return CompilationFailed( ctx );
            }
        }

        // Serialize
        //-------------------------------------------------------------------------

        Resource::ResourceHeader hdr( s_version, PhysicsMaterialDatabase::GetStaticResourceTypeID() );

        Serialization::BinaryOutputArchive archive;
        archive << hdr << materialSettings;

        if ( archive.WriteToFile( ctx.m_outputFilePath ) )
        {
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}