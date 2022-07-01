#pragma once

#include "System/_Module/API.h"
#include "System/Types/Arrays.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class RenderDevice;
    class Texture;

    namespace DefaultResources
    {
        void Initialize( RenderDevice* pRenderDevice );
        void Shutdown( RenderDevice* pRenderDevice );

        KRG_SYSTEM_API Texture const* GetDefaultTexture();
    };
}