#pragma once

#include "Game/_Module/API.h"
#include "Engine/Entity/EntitySpatialComponent.h"
#include "Engine/Entity/EntityDescriptors.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class KRG_ENGINE_API AISpawnComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AISpawnComponent );

    public:

        inline AISpawnComponent() = default;

        inline EntityModel::EntityCollectionDescriptor const* GetEntityCollectionDesc() const { return m_pAIEntityDesc.GetPtr(); }

    private:

        KRG_EXPOSE TResourcePtr<EntityModel::EntityCollectionDescriptor>    m_pAIEntityDesc = nullptr;
    };
}