#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Entity/EntityWorldSystem.h"
#include "System/Types/IDVector.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;

    //-------------------------------------------------------------------------

    class AnimationWorldSystem : public IWorldEntitySystem
    {
        friend class AnimationDebugView;

    public:

        KRG_REGISTER_TYPE( AnimationWorldSystem );
        KRG_ENTITY_WORLD_SYSTEM( AnimationWorldSystem );

    private:

        virtual void ShutdownSystem() override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

    private:

        TIDVector<ComponentID, AnimationGraphComponent*>          m_graphComponents;
    };
} 