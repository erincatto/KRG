#include "AIBehavior.h"
#include "Game/AI/AIAnimationController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Drawing/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    BehaviorContext::~BehaviorContext()
    {
        KRG_ASSERT( m_pEntityWorldUpdateContext == nullptr && m_pNavmeshSystem == nullptr && m_pPhysicsScene == nullptr );
        KRG_ASSERT( m_pCharacter == nullptr && m_pCharacterController == nullptr );
        KRG_ASSERT( m_pAIComponent == nullptr && m_pAnimationController == nullptr );
    }

    bool BehaviorContext::IsValid() const
    {
        if ( m_pAIComponent == nullptr )
        {
            return false;
        }

        if ( m_pCharacter == nullptr || m_pCharacterController == nullptr || !m_pCharacter->IsRootComponent() )
        {
            return false;
        }

        if ( m_pAnimationController == nullptr || !m_pAnimationController->HasSubGraphControllers() )
        {
            return false;
        }

        return m_pEntityWorldUpdateContext != nullptr && m_pNavmeshSystem != nullptr && m_pPhysicsScene != nullptr;
    }

    Drawing::DrawContext BehaviorContext::GetDrawingContext() const
    {
        return m_pEntityWorldUpdateContext->GetDrawingContext();
    }

}