#pragma once
#include "Engine/_Module/API.h"
#include "System/Systems.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavPowerAllocator;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API NavmeshSystem final : public ISystem
    {
        friend class NavmeshWorldSystem;

    public:

        KRG_SYSTEM_ID( NavmeshSystem );

    public:

        NavmeshSystem();
        ~NavmeshSystem();

    private:

        NavPowerAllocator*                              m_pAllocator = nullptr;
    };
}