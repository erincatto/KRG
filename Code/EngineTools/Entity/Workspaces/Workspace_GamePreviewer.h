#pragma once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Core/Workspaces/EditorWorkspace.h"
#include "Engine/ToolsUI/EngineToolsUI.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_ENGINETOOLS_API GamePreviewer final : public EditorWorkspace
    {
    public:

        GamePreviewer( ToolsContext const* pToolsContext, EntityWorld* pWorld );

        void LoadMapToPreview( ResourceID mapResourceID );

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        virtual uint32_t GetID() const override { return 0xFFFFFFFE; }
        virtual bool HasWorkspaceToolbar() const override;
        virtual bool HasViewportToolbar() const override { return false; }
        virtual bool HasViewportOrientationGuide() const override { return false; }
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;

        virtual void DrawWorkspaceToolbarItems( UpdateContext const& context ) override;
        virtual void UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual bool HasWorkspaceToolbarDefaultItems() const override { return false; }

    private:

        ResourceID                      m_loadedMap;
        EngineToolsUI                   m_engineToolsUI;
    };
}