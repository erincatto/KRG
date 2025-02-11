#include "PlayerAction.h"
#include "Game/Player/Components/Component_MainPlayer.h"
#include "Game/Player/PlayerPhysicsController.h"
#include "Game/Player/PlayerAnimationController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Drawing/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    ActionContext::~ActionContext()
    {
        KRG_ASSERT( m_pEntityWorldUpdateContext == nullptr && m_pInputState == nullptr && m_pPhysicsScene == nullptr && m_pCharacterController == nullptr );
        KRG_ASSERT( m_pCharacterComponent == nullptr && m_pCharacterController == nullptr );
        KRG_ASSERT( m_pPlayerComponent == nullptr && m_pAnimationController == nullptr && m_pCameraController == nullptr );
    }

    bool ActionContext::IsValid() const
    {
        if ( m_pPlayerComponent == nullptr )
        {
            return false;
        }

        if ( m_pCharacterComponent == nullptr || m_pCharacterController == nullptr || !m_pCharacterComponent->IsRootComponent() )
        {
            return false;
        }

        if ( m_pAnimationController == nullptr || !m_pAnimationController->HasSubGraphControllers() )
        {
            return false;
        }

        return m_pEntityWorldUpdateContext != nullptr && m_pCameraController != nullptr && m_pInputState != nullptr && m_pPhysicsScene != nullptr && m_pCharacterController != nullptr;
    }

    #if KRG_DEVELOPMENT_TOOLS
    Drawing::DrawContext ActionContext::GetDrawingContext() const
    {
        return m_pEntityWorldUpdateContext->GetDrawingContext();
    }
    #endif
}