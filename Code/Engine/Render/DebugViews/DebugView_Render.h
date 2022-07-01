#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Render
{
    class RendererWorldSystem;
    class LightComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API RenderDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( RenderDebugView );

    public:

        static void DrawRenderVisualizationModesMenu( EntityWorld const* pWorld );

    public:

        RenderDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawOverlayElements( EntityWorldUpdateContext const& context ) override;

        void DrawRenderMenu( EntityWorldUpdateContext const& context );

    private:

        RendererWorldSystem*            m_pWorldRendererSystem = nullptr;
    };
}
#endif