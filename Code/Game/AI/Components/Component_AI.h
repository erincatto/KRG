#pragma once

#include "Game/_Module/API.h"
#include "Engine/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------
// AI Component
//-------------------------------------------------------------------------
// This component identifies all AI entities

namespace KRG::AI
{
    class KRG_GAME_API AIComponent : public EntityComponent
    {
        KRG_REGISTER_SINGLETON_ENTITY_COMPONENT( AIComponent );

    public:

        inline AIComponent() = default;
        inline AIComponent( StringID name ) : EntityComponent( name ) {}
    };
}