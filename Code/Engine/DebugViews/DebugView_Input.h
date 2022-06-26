#pragma once

#include "Engine/Entity/EntityWorldDebugView.h"
#include "System/Input/InputDevices/InputDevice_Controller.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Input
{
    class ControllerInputDevice;
    class InputSystem;

    //-------------------------------------------------------------------------

    class InputDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( InputDebugView );

    public:

        InputDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawControllerMenu( EntityWorldUpdateContext const& context );
        void DrawControllerState( ControllerInputDevice const& controllerState );

    private:

        InputSystem*                                   m_pInputSystem = nullptr;
        TVector<ControllerInputDevice const*>          m_openControllerWindows;
    };
}
#endif