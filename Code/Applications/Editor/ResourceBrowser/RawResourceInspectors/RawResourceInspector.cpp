#include "RawResourceInspector.h"
#include "EngineTools/Resource/Compilers/ResourceDescriptor.h"
#include "EngineTools/Resource/ResourceDatabase.h"
#include "EngineTools/ThirdParty/pfd/portable-file-dialogs.h"
#include "EngineTools/Core/ToolsContext.h"
#include "System/FileSystem/FileSystem.h"
#include "System/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    RawResourceInspector::RawResourceInspector( ToolsContext const* pToolsContext, FileSystem::Path const& filePath )
        : m_pToolsContext( pToolsContext )
        , m_rawResourceDirectory( pToolsContext->m_pResourceDatabase->GetRawResourceDirectoryPath() )
        , m_filePath( filePath )
        , m_propertyGrid( pToolsContext )
    {
        KRG_ASSERT( m_rawResourceDirectory.IsDirectoryPath() && FileSystem::Exists( m_rawResourceDirectory ) );
        KRG_ASSERT( filePath.IsFilePath() && FileSystem::Exists( filePath ) );
    }

    RawResourceInspector::~RawResourceInspector()
    {
        KRG::Delete( m_pDescriptor );
    }

    bool RawResourceInspector::DrawDialog()
    {
        if ( !ImGui::IsPopupOpen( GetInspectorTitle() ) )
        {
            ImGui::OpenPopup( GetInspectorTitle() );
        }

        //-------------------------------------------------------------------------

        bool isOpen = true;
        ImGui::SetNextWindowSize( ImVec2( 600, 800 ), ImGuiCond_FirstUseEver );
        if ( ImGui::BeginPopupModal( GetInspectorTitle(), &isOpen ) )
        {
            DrawFileInfo();

            //-------------------------------------------------------------------------

            auto availableSpace = ImGui::GetContentRegionAvail();
            if ( ImGui::BeginTable( "DialogTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_PadOuterX | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_ScrollY, availableSpace ) )
            {
                ImGui::TableSetupColumn( "Info", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Creator", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch );

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0);
                if ( ImGui::BeginChild( "Contents", ImGui::GetContentRegionAvail() - ImVec2( 0, 4 ) ) )
                {
                    DrawFileContents();
                }
                ImGui::EndChild();

                ImGui::TableSetColumnIndex( 1);
                DrawResourceDescriptorCreator();

                ImGui::EndTable();
            }

            ImGui::EndPopup();
        }

        return isOpen && ImGui::IsPopupOpen( GetInspectorTitle() );
    }

    bool RawResourceInspector::CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const* pDescriptor ) const
    {
        KRG_ASSERT( resourceTypeID.IsValid() );

        //-------------------------------------------------------------------------

        TInlineString<5> extension = resourceTypeID.ToString();
        extension.make_lower();

        FileSystem::Path newDescriptorPath = m_filePath;
        newDescriptorPath.ReplaceExtension( extension );

        //-------------------------------------------------------------------------

        TInlineString<10> const filter( TInlineString<10>::CtorSprintf(), "*.%s", extension.c_str() );
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

        Serialization::TypeWriter typeWriter( *m_pToolsContext->m_pTypeRegistry );
        typeWriter << pDescriptor;
        return typeWriter.WriteToFile( newDescriptorPath );
    }

    //-------------------------------------------------------------------------

    KRG_DEFINE_GLOBAL_REGISTRY( RawResourceInspectorFactory );

    //-------------------------------------------------------------------------

    bool RawResourceInspectorFactory::CanCreateInspector( FileSystem::Path const& filePath )
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

    RawResourceInspector* RawResourceInspectorFactory::TryCreateInspector( ToolsContext const* pToolsContext, FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( pCurrentFactory->IsSupportedFile( filePath ) )
            {
                return pCurrentFactory->CreateInspector( pToolsContext, filePath );
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return nullptr;
    }
}