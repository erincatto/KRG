#include "NavmeshSystem.h"
#include "NavPower.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    NavmeshSystem::NavmeshSystem()
    {
        #if KRG_ENABLE_NAVPOWER
        m_pAllocator = KRG::New<NavPowerAllocator>();
        #endif
    }

    NavmeshSystem::~NavmeshSystem()
    {
        #if KRG_ENABLE_NAVPOWER
        KRG::Delete( m_pAllocator );
        #endif
    }
}