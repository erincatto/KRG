#pragma once

#include "ResourceDescriptor.h"
#include "System/Resource/ResourceHeader.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Types/Function.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    enum class CompilationResult
    {
        Failure = -1,
        Success = 0,
        SuccessWithWarnings = 1,
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_CORE_API CompileContext
    {
        CompileContext( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath, FileSystem::Path const& compiledResourceDirectoryPath, ResourceID const& resourceToCompile );

        bool IsValid() const;

        inline bool ConvertResourcePathToFilePath( ResourcePath const& resourcePath, FileSystem::Path& filePath ) const
        {
            if ( resourcePath.IsValid() )
            {
                filePath = ResourcePath::ToFileSystemPath( m_rawResourceDirectoryPath, resourcePath );
                return true;
            }
            else
            {
                KRG_LOG_ERROR( "ResourceCompiler", "Invalid data path encountered: '%s'", resourcePath.c_str() );
                return false;
            }
        }

    public:

        TypeSystem::TypeRegistry const&                 m_typeRegistry;
        Platform::Target const                          m_platform = Platform::Target::PC;
        FileSystem::Path const                          m_rawResourceDirectoryPath;
        FileSystem::Path const                          m_compiledResourceDirectoryPath;

        ResourceID const                                m_resourceID;
        FileSystem::Path const                          m_inputFilePath;
        FileSystem::Path const                          m_outputFilePath;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API Compiler
    {

    public:

        Compiler( String const& name, int32_t version ) : m_name( name ), m_version( version ) {}
        virtual ~Compiler() {}
        virtual CompilationResult Compile( CompileContext const& ctx ) const = 0;

        String const& GetName() const { return m_name; }
        inline int32_t GetVersion() const { return m_version; }

        // The list of resource type we can compile
        virtual TVector<ResourceTypeID> const& GetOutputTypes() const { return m_outputTypes; }

        // The list of virtual resources we produce as part of the compilation process
        virtual TVector<ResourceTypeID> const& GetVirtualTypes() const { return m_virtualTypes; }

    protected:

        Compiler& operator=( Compiler const& ) = delete;

        CompilationResult Error( char const* pFormat, ... ) const;
        CompilationResult Warning( char const* pFormat, ... ) const;
        CompilationResult Message( char const* pFormat, ... ) const;

        CompilationResult CompilationSucceeded( CompileContext const& ctx ) const;
        CompilationResult CompilationSucceededWithWarnings( CompileContext const& ctx ) const;
        CompilationResult CompilationFailed( CompileContext const& ctx ) const;

    protected:

        int32_t                                           m_version;
        String                                          m_name;
        TVector<ResourceTypeID>                         m_outputTypes;
        TVector<ResourceTypeID>                         m_virtualTypes;
    };
}