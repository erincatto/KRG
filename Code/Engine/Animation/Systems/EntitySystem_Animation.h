#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Transform;
    class SpatialEntityComponent;
}

namespace KRG::Render
{
    class SkeletalMeshComponent;
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;
    class AnimationClipPlayerComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API AnimationSystem : public EntitySystem
    {
        KRG_REGISTER_ENTITY_SYSTEM( AnimationSystem, RequiresUpdate( UpdateStage::PrePhysics ), RequiresUpdate( UpdateStage::PostPhysics, UpdatePriority::Low ) );

    public:

        virtual ~AnimationSystem();

    private:

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityWorldUpdateContext const& ctx ) override;

        void UpdateAnimPlayers( EntityWorldUpdateContext const& ctx, Transform const& characterWorldTransform );
        void UpdateAnimGraphs( EntityWorldUpdateContext const& ctx, Transform const& characterWorldTransform );

    private:

        TVector<AnimationClipPlayerComponent*>          m_animPlayers;
        TVector<AnimationGraphComponent*>               m_animGraphs;
        TVector<Render::SkeletalMeshComponent*>         m_meshComponents;
        SpatialEntityComponent*                         m_pRootComponent = nullptr;
    };
}