#if _WIN32
#pragma once

#include "System/_Module/API.h"
#include "System/KRG.h"

//-------------------------------------------------------------------------

struct HWND__;

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::Platform
{
    void InitializePlatform();
    void ShutdownPlatform();

    // Called each frame
    void UpdateDisplayInformation();
    void UpdateInputInformation();

    // Returns 0 when the message isnt handled, used to embed into another wnd proc
    KRG_SYSTEM_API intptr_t WindowsMessageHandler( HWND__* hWnd, uint32_t message, uintptr_t wParam, intptr_t lParam );
}
#endif