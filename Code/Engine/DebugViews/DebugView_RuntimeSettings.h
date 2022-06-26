#pragma once

#include "Engine/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class UpdateContext;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API RuntimeSettingsDebugView final : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( RuntimeSettingsDebugView );

    public:

        static bool DrawRuntimeSettingsView( UpdateContext const& context );

    public:

        RuntimeSettingsDebugView();

    private:

        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override {};
        void DrawMenu( EntityWorldUpdateContext const& context );
    };
}
#endif