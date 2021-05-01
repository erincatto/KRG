#pragma once

#include "MeshToolkit_DataBrowser.h"
#include "Applications/Editor/Editor/EditorToolkit.h"
#include "Applications/Editor/Common/DocumentWell.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    class MeshToolkit final : public TEditorToolkit<EditorModel>
    {
    public:

        ~MeshToolkit();

    private:

        virtual char const* GetName() const override final { return "Kruger Mesh Toolkit"; }

        virtual void SetUserFlags( uint64 flags ) override;
        virtual uint64 GetUserFlags() const override;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        DataBrowser*                        m_pDataBrowser = nullptr;
        DocumentWell*                       m_pDocumentWell = nullptr;
    };
}