#pragma once

#include "Game/_Module/API.h"
#include "Game/Player/PlayerActionStateMachine.h"
#include "Engine/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class OrbitCameraComponent;

    namespace Render
    {
        class CharacterMeshComponent;
    }

    namespace Animation
    {
        class AnimationGraphComponent;
    }
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class KRG_GAME_API PlayerController final : public EntitySystem
    {
        friend class PlayerDebugView;

        KRG_REGISTER_ENTITY_SYSTEM( PlayerController, RequiresUpdate( UpdateStage::PrePhysics, UpdatePriority::Highest ), RequiresUpdate( UpdateStage::PostPhysics, UpdatePriority::Highest ) );

    private:

        virtual void Activate() override;
        virtual void Shutdown() override;

        virtual void RegisterComponent( EntityComponent* pComponent ) override;
        virtual void UnregisterComponent( EntityComponent* pComponent ) override;
        virtual void Update( EntityWorldUpdateContext const& ctx ) override;

    private:

        ActionContext                                           m_actionContext;
        ActionStateMachine                                      m_actionStateMachine = ActionStateMachine( m_actionContext );

        Animation::AnimationGraphComponent*                     m_pAnimGraphComponent = nullptr;
        Render::CharacterMeshComponent*                         m_pCharacterMeshComponent = nullptr;
        OrbitCameraComponent*                                   m_pCameraComponent = nullptr;
    };
}