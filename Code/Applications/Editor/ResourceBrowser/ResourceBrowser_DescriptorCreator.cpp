#include "ResourceBrowser_DescriptorCreator.h"
#include "Applications/Editor/EditorContext.h"
#include "EngineTools/ThirdParty/pfd/portable-file-dialogs.h"
#include "EngineTools/Resource/Compilers/ResourceDescriptor.h"
#include "EngineTools/Core/Helpers/CommonDialogs.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Math/MathStringHelpers.h"
#include "System/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceDescriptorCreator::ResourceDescriptorCreator( EditorContext* pEditorContext, TypeSystem::TypeID const descriptorTypeID, FileSystem::Path const& startingDir )
        : m_pEditorContext( pEditorContext )
        , m_propertyGrid( pEditorContext )
        , m_startingPath( startingDir )
    {
        KRG_ASSERT( m_pEditorContext != nullptr );
        KRG_ASSERT( m_pEditorContext->GetTypeRegistry()->IsTypeDerivedFrom( descriptorTypeID, Resource::ResourceDescriptor::GetStaticTypeID() ) );
        auto pTypeInfo = m_pEditorContext->GetTypeRegistry()->GetTypeInfo( descriptorTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        m_pDescriptor = Cast<Resource::ResourceDescriptor>( pTypeInfo->m_pTypeHelper->CreateType() );
        KRG_ASSERT( m_pDescriptor != nullptr );

        m_propertyGrid.SetTypeToEdit( m_pDescriptor );

        //-------------------------------------------------------------------------

        TInlineString<10> const filenameStr( TInlineString < 10>::CtorSprintf(), "NewResource.%s", m_pDescriptor->GetCompiledResourceTypeID().ToString().c_str() );
        m_startingPath += filenameStr.c_str();
    }

    ResourceDescriptorCreator::~ResourceDescriptorCreator()
    {
        KRG::Delete( m_pDescriptor );
    }

    bool ResourceDescriptorCreator::Draw()
    {
        if ( !ImGui::IsPopupOpen( s_title ) )
        {
            ImGui::OpenPopup( s_title );
        }

        //-------------------------------------------------------------------------

        bool isOpen = true;
        ImGui::SetNextWindowSize( ImVec2( 600, 800 ), ImGuiCond_FirstUseEver );
        if ( ImGui::BeginPopupModal( s_title, &isOpen ) )
        {
            m_propertyGrid.DrawGrid();

            //-------------------------------------------------------------------------

            if ( ImGui::Button( "Save", ImVec2( 120, 0 ) ) )
            {
                SaveDescriptor();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if ( ImGui::Button( "Cancel", ImVec2( 120, 0 ) ) )
            {
                ImGui::CloseCurrentPopup();
            }

            //-------------------------------------------------------------------------

            ImGui::EndPopup();
        }

        return isOpen && ImGui::IsPopupOpen( s_title );
    }

    bool ResourceDescriptorCreator::SaveDescriptor()
    {
        auto pTypeRegistry = m_pEditorContext->GetTypeRegistry();

        //-------------------------------------------------------------------------

        ResourceTypeID const resourceTypeID = m_pDescriptor->GetCompiledResourceTypeID();
        TInlineString<5> const resourceTypeIDString = resourceTypeID.ToString();
        TypeSystem::ResourceInfo const* pResourceInfo = pTypeRegistry->GetResourceInfoForResourceType( resourceTypeID );

        FileSystem::Path outPath = SaveDialog( resourceTypeID, m_startingPath, pResourceInfo->m_friendlyName );
        if ( !outPath.IsValid() )
        {
            return false;
        }

        // Ensure that the extension matches the expected type
        auto const extension = outPath.GetExtensionAsString();
        if ( extension != resourceTypeIDString.c_str() )
        {
            outPath.ReplaceExtension( resourceTypeIDString.c_str() );
        }

        KRG_ASSERT( m_pDescriptor != nullptr );
        return Resource::ResourceDescriptor::TryWriteToFile( *pTypeRegistry, outPath, m_pDescriptor );
    }
}