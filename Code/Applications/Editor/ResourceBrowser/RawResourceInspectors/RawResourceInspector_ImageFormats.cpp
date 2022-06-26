#include "RawResourceInspector_ImageFormats.h"
#include "EngineTools/RawAssets/RawAssetReader.h"
#include "EngineTools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"
#include "System/Imgui/ImguiX.h"
#include "System/Math/MathStringHelpers.h"
#include "System/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryPNG, "png", ResourceInspectorImageFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryBMP, "bmp", ResourceInspectorImageFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryTGA, "tga", ResourceInspectorImageFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryJPG, "jpg", ResourceInspectorImageFormats );

    //-------------------------------------------------------------------------

    ResourceInspectorImageFormats::ResourceInspectorImageFormats( ToolsContext const* pToolsContext, FileSystem::Path const& filePath )
        : RawResourceInspector( pToolsContext, filePath )
    {
        KRG_ASSERT( FileSystem::Exists( filePath ) );
        auto pDesc = KRG::New<Render::TextureResourceDescriptor>();
        pDesc->m_path = ResourcePath::FromFileSystemPath( m_rawResourceDirectory, m_filePath );
        m_pDescriptor = pDesc;

        m_propertyGrid.SetTypeToEdit( m_pDescriptor );
    }

    void ResourceInspectorImageFormats::DrawFileInfo()
    {
        ImGui::Text( m_filePath.c_str() );
    }

    void ResourceInspectorImageFormats::DrawFileContents()
    {
    }

    void ResourceInspectorImageFormats::DrawResourceDescriptorCreator()
    {
        if ( ImGui::Button( KRG_ICON_PLUS " Create Texture", ImVec2( -1, 0 ) ) )
        {
            CreateNewDescriptor( Render::Texture::GetStaticResourceTypeID(), m_pDescriptor );
        }
    }
}