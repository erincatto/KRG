#include "RawFileInspector.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"

//-------------------------------------------------------------------------

namespace KRG
{
    RawFileInspector::RawFileInspector( EditorModel* pModel, FileSystem::Path const& filePath )
        : m_filePath( filePath )
        , m_pModel( pModel )
    {
        KRG_ASSERT( filePath.IsValid() && filePath.IsExistingFile() );
    }

    bool RawFileInspector::CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const& descriptor ) const
    {
        KRG_ASSERT( resourceTypeID.IsValid() );

        //-------------------------------------------------------------------------

        InlineString<5> extension = resourceTypeID.ToString();
        extension.make_lower();

        FileSystem::Path newDescriptorPath = m_filePath;
        newDescriptorPath.ReplaceExtension( extension );

        InlineString<10> filter;
        filter.sprintf( "*.%s", extension.c_str() );

        //-------------------------------------------------------------------------

        pfd::save_file saveDialog( "Save Resource Descriptor", newDescriptorPath.c_str(), { "Descriptor", filter.c_str() } );
        newDescriptorPath = saveDialog.result().c_str();

        if ( !newDescriptorPath.IsValid() || !newDescriptorPath.IsFilePath() )
        {
            return false;
        }

        // Ensure correct extension
        if ( !newDescriptorPath.MatchesExtension( extension.c_str() ) )
        {
            newDescriptorPath.Append( "." );
            newDescriptorPath.Append( extension.c_str() );
        }

        //-------------------------------------------------------------------------

        TypeSystem::Serialization::TypeWriter typeWriter( *m_pModel->GetTypeRegistry() );
        typeWriter << &descriptor;
        return typeWriter.WriteToFile( newDescriptorPath );
    }

    //-------------------------------------------------------------------------

    KRG_DEFINE_GLOBAL_REGISTRY( RawFileInspectorFactory );

    //-------------------------------------------------------------------------

    bool RawFileInspectorFactory::CanCreateInspector( FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( pCurrentFactory->IsSupportedFile( filePath ) )
            {
                return true;
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return false;
    }

    RawFileInspector* RawFileInspectorFactory::TryCreateInspector( EditorModel* pModel, FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( pCurrentFactory->IsSupportedFile( filePath ) )
            {
                return pCurrentFactory->CreateInspector( pModel, filePath );
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return nullptr;
    }
}