#pragma once
#include "RawResourceInspector.h"
#include "EngineTools/RawAssets/gltf/gltfSceneContext.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceInspectorGLTF : public RawResourceInspector
    {
    public:

        ResourceInspectorGLTF( ToolsContext const* pToolsContext, FileSystem::Path const& filePath );

    private:

        virtual char const* GetInspectorTitle() const override { return  "GLTF Inspector"; }
        virtual void DrawFileInfo() override;
        virtual void DrawFileContents() override;
        virtual void DrawResourceDescriptorCreator() override;

    private:

        gltf::gltfSceneContext        m_sceneContext;
    };
}