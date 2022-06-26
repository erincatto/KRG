#pragma once
#include "Engine/_Module/API.h"
#include "System/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG::ImGuiX::OrientationGuide
{
    KRG_ENGINE_API Float2 GetSize();
    KRG_ENGINE_API float GetWidth();
    KRG_ENGINE_API void Draw( Float2 const& guideOrigin, Render::Viewport const& viewport );
}