#pragma once
#include "RawResourceInspector.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceInspectorImageFormats : public RawResourceInspector
    {
    public:

        ResourceInspectorImageFormats( ToolsContext const* pToolsContext, FileSystem::Path const& filePath );

    private:

        virtual char const* GetInspectorTitle() const override { return  "Image Inspector"; }
        virtual void DrawFileInfo() override;
        virtual void DrawFileContents() override;
        virtual void DrawResourceDescriptorCreator() override;
    };
}