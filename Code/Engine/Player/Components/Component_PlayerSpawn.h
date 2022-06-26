#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Entity/EntitySpatialComponent.h"
#include "Engine/Entity/EntityDescriptors.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------
// Player Spawn Component
//-------------------------------------------------------------------------
// This component defines the spawning location for the player in the game world

namespace KRG::Player
{
    class KRG_ENGINE_API PlayerSpawnComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PlayerSpawnComponent );

    public:

        inline PlayerSpawnComponent() = default;

        inline EntityModel::EntityCollectionDescriptor const* GetEntityCollectionDesc() const { return m_pPlayerEntityDesc.GetPtr(); }

    private:

        KRG_EXPOSE TResourcePtr<EntityModel::EntityCollectionDescriptor>    m_pPlayerEntityDesc = nullptr;
    };
}