#include "AIBehaviorSelector.h"
#include "System/Log.h"
#include "Behaviors/AIBehavior_Wander.h"
#include "Behaviors/AIBehavior_CombatPositioning.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    BehaviorSelector::BehaviorSelector( BehaviorContext const& context )
        : m_actionContext( context )
    {
        m_behaviors.emplace_back( KRG::New<WanderBehavior>() );
        m_behaviors.emplace_back( KRG::New<CombatPositionBehavior>() );
    }

    BehaviorSelector::~BehaviorSelector()
    {
        for ( auto& pBehavior : m_behaviors )
        {
            KRG::Delete( pBehavior );
        }
        m_behaviors.clear();
    }

    //-------------------------------------------------------------------------

    void BehaviorSelector::Update()
    {
        KRG_ASSERT( m_actionContext.IsValid() );

        //-------------------------------------------------------------------------

        // HACK HACK HACK
        if ( m_pActiveBehavior == nullptr )
        {
            m_pActiveBehavior = m_behaviors.back();
            m_pActiveBehavior->Start( m_actionContext );
        }
        // HACK HACK HACK

        //-------------------------------------------------------------------------

        if ( m_pActiveBehavior != nullptr )
        {
            m_pActiveBehavior->Update( m_actionContext );
        }
    }
}