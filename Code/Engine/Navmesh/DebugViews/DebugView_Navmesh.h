#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Navmesh
{
    class NavmeshSystem;
    class NavmeshWorldSystem;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API NavmeshDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( NavmeshDebugView );

    public:

        static void DrawNavmeshRuntimeSettings( NavmeshWorldSystem* pNavmeshWorldSystem );

    public:

        NavmeshDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityWorldUpdateContext const& context );

    private:

        NavmeshWorldSystem*     m_pNavmeshWorldSystem = nullptr;
    };
}
#endif