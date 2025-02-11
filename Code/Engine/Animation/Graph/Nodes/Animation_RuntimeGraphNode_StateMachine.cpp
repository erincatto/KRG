#include "Animation_RuntimeGraphNode_StateMachine.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Contexts.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void StateMachineNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<StateMachineNode>( nodePtrs, options );

        for ( auto& stateSettings : m_stateSettings )
        {
            StateInfo& state = pNode->m_states.emplace_back();
            SetNodePtrFromIndex( nodePtrs, stateSettings.m_stateNodeIdx, state.m_pStateNode );
            SetOptionalNodePtrFromIndex( nodePtrs, stateSettings.m_entryConditionNodeIdx, state.m_pEntryConditionNode );

            for ( auto& transitionSettings : stateSettings.m_transitionSettings )
            {
                TransitionInfo& transition = state.m_transitions.emplace_back();
                transition.m_targetStateIdx = transitionSettings.m_targetStateIdx;
                SetNodePtrFromIndex( nodePtrs, transitionSettings.m_transitionNodeIdx, transition.m_pTransitionNode );
                SetNodePtrFromIndex( nodePtrs, transitionSettings.m_conditionNodeIdx, transition.m_pConditionNode );
            }
        }
    }

    bool StateMachineNode::StateInfo::HasForceableTransitions() const
    {
        for ( auto const& transition : m_transitions )
        {
            if ( transition.m_pTransitionNode->IsForcedTransitionAllowed() )
            {
                return true;
            }
        }

        return false;
    }

    //-------------------------------------------------------------------------

    SyncTrack const& StateMachineNode::GetSyncTrack() const
    {
        KRG_ASSERT( IsValid() );
        if ( m_pActiveTransition != nullptr )
        {
            return m_pActiveTransition->GetSyncTrack();
        }
        else
        {
            return m_states[m_activeStateIndex].m_pStateNode->GetSyncTrack();
        }
    }

    bool StateMachineNode::IsValid() const
    {
        return PoseNode::IsValid() && m_activeStateIndex >= 0 && m_activeStateIndex < m_states.size();
    }

    StateMachineNode::StateIndex StateMachineNode::SelectDefaultState( GraphContext& context ) const
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<StateMachineNode>();

        StateIndex selectedStateIndex = pSettings->m_defaultStateIndex;
        auto const numStates = (int16_t) m_states.size();

        // NOTE: we need to initialize all conditions in advance to ensure that the value caching works
        for ( auto i = 0; i < numStates; i++ )
        {
            if ( m_states[i].m_pEntryConditionNode != nullptr )
            {
                m_states[i].m_pEntryConditionNode->Initialize( context );
            }
        }

        // Check all conditions
        for ( StateIndex i = 0; i < numStates; i++ )
        {
            if ( m_states[i].m_pEntryConditionNode != nullptr )
            {
                if ( m_states[i].m_pEntryConditionNode->GetValue<bool>( context ) )
                {
                    selectedStateIndex = i;
                    break;
                }
            }
        }

        // Shutdown all conditions
        for ( auto i = 0; i < numStates; i++ )
        {
            if ( m_states[i].m_pEntryConditionNode != nullptr )
            {
                m_states[i].m_pEntryConditionNode->Shutdown( context );
            }
        }

        return selectedStateIndex;
    }

    void StateMachineNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() && m_activeStateIndex == InvalidIndex && m_pActiveTransition == nullptr );

        PoseNode::InitializeInternal( context, initialTime );

        // Determine default state and initialize it
        m_activeStateIndex = SelectDefaultState( context );
        KRG_ASSERT( m_activeStateIndex != InvalidIndex );
        StateNode* ActiveState = m_states[m_activeStateIndex].m_pStateNode;
        ActiveState->Initialize( context, initialTime );

        // Initialize the base animation graph node
        m_duration = ActiveState->GetDuration();
        m_previousTime = ActiveState->GetPreviousTime();
        m_currentTime = ActiveState->GetCurrentTime();

        InitializeTransitionConditions( context );
    }

    void StateMachineNode::InitializeTransitionConditions( GraphContext& context )
    {
        for ( auto const& transition : m_states[m_activeStateIndex].m_transitions )
        {
            if ( transition.m_pConditionNode != nullptr )
            {
                transition.m_pConditionNode->Initialize( context );
            }
        }
    }

    void StateMachineNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_activeStateIndex != InvalidIndex );

        if ( m_pActiveTransition != nullptr )
        {
            KRG_ASSERT( m_pActiveTransition->IsInitialized() );
            m_pActiveTransition->Shutdown( context );
        }

        ShutdownTransitionConditions( context );

        m_states[m_activeStateIndex].m_pStateNode->Shutdown( context );
        m_activeStateIndex = InvalidIndex;
        m_pActiveTransition = nullptr;

        PoseNode::ShutdownInternal( context );
    }

    void StateMachineNode::ShutdownTransitionConditions( GraphContext& context )
    {
        for ( auto const& transition : m_states[m_activeStateIndex].m_transitions )
        {
            if ( transition.m_pConditionNode != nullptr )
            {
                transition.m_pConditionNode->Shutdown( context );
            }
        }
    }

    void StateMachineNode::EvaluateTransitions( GraphContext& context, GraphPoseNodeResult& sourceNodeResult )
    {
        auto pSettings = GetSettings<StateMachineNode>();
        auto const& currentlyActiveState = m_states[m_activeStateIndex];

        //-------------------------------------------------------------------------
        // Check for a valid transition
        //-------------------------------------------------------------------------

        int32_t transitionIdx = InvalidIndex;
        bool shouldForceTransition = false;

        int32_t const numTransitions = (int32_t) currentlyActiveState.m_transitions.size();
        for ( int32_t i = 0; i < numTransitions; i++ )
        {
            auto const& transition = currentlyActiveState.m_transitions[i];
            KRG_ASSERT( transition.m_targetStateIdx != InvalidIndex );

            // Disallow any transitions to already transitioning states unless this is a forced transition, this will prevent infinite transition loops
            if ( !transition.m_pTransitionNode->IsForcedTransitionAllowed() && m_states[transition.m_targetStateIdx].m_pStateNode->IsTransitioning() )
            {
                continue;
            }

            // Check if the conditions for this transition are satisfied, if they are start a new transition
            if ( transition.m_pConditionNode != nullptr && transition.m_pConditionNode->GetValue<bool>( context ) )
            {
                if ( m_states[transition.m_targetStateIdx].m_pStateNode->IsTransitioning() )
                {
                    shouldForceTransition = true;
                }

                transitionIdx = i;
                break;
            }
        }

        //-------------------------------------------------------------------------
        // Start new transition
        //-------------------------------------------------------------------------

        if ( transitionIdx != InvalidIndex )
        {
            KRG_ASSERT( transitionIdx >= 0 && transitionIdx < currentlyActiveState.m_transitions.size() );

            auto const& transition = currentlyActiveState.m_transitions[transitionIdx];

            TransitionNode::InitializationOptions initOptions;
            initOptions.m_sourceNodeResult = sourceNodeResult;
            initOptions.m_shouldCachePose = m_states[transition.m_targetStateIdx].HasForceableTransitions();

            // If we are fully in a state, so use this state as the source else use the currently active transition
            // Note: the initialization of the transition will update the target state
            if ( m_pActiveTransition != nullptr )
            {
                sourceNodeResult = transition.m_pTransitionNode->StartTransitionFromTransition( context, initOptions, m_pActiveTransition, shouldForceTransition );
            }
            else // Basic transition
            {
                sourceNodeResult = transition.m_pTransitionNode->StartTransitionFromState( context, initOptions, m_states[m_activeStateIndex].m_pStateNode );
            }

            m_pActiveTransition = transition.m_pTransitionNode;

            // Update state data to that of the new active state
            ShutdownTransitionConditions( context );
            m_activeStateIndex = transition.m_targetStateIdx;
            InitializeTransitionConditions( context );

            m_duration = m_states[m_activeStateIndex].m_pStateNode->GetDuration();
            m_previousTime = m_states[m_activeStateIndex].m_pStateNode->GetPreviousTime();
            m_currentTime = m_states[m_activeStateIndex].m_pStateNode->GetCurrentTime();
        }
    }

    void StateMachineNode::UpdateTransitionStack( GraphContext& context )
    {
        if ( m_pActiveTransition == nullptr )
        {
            return;
        }

        if ( m_pActiveTransition->IsComplete() )
        {
            m_pActiveTransition->Shutdown( context );
            m_pActiveTransition = nullptr;
        }
    }

    GraphPoseNodeResult StateMachineNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );

        // Update transitions
        UpdateTransitionStack( context );

        // If we are fully in a state, update the state directly
        GraphPoseNodeResult result;
        if ( m_pActiveTransition == nullptr )
        {
            result = static_cast<PoseNode*>( m_states[m_activeStateIndex].m_pStateNode )->Update( context );

            // Update node time
            m_duration = m_states[m_activeStateIndex].m_pStateNode->GetDuration();
            m_previousTime = m_states[m_activeStateIndex].m_pStateNode->GetPreviousTime();
            m_currentTime = m_states[m_activeStateIndex].m_pStateNode->GetCurrentTime();
        }
        else // Update the transition
        {
            result = static_cast<PoseNode*>( m_pActiveTransition )->Update( context );

            // Update node time
            m_duration = m_pActiveTransition->GetDuration();
            m_previousTime = m_pActiveTransition->GetPreviousTime();
            m_currentTime = m_pActiveTransition->GetCurrentTime();
        }

        KRG_ASSERT( context.m_sampledEvents.IsValidRange( result.m_sampledEventRange ) );

        // Check for a valid transition, if one exists start it
        if ( context.m_branchState == BranchState::Active )
        {
            EvaluateTransitions( context, result );
        }

        return result;
    }

    GraphPoseNodeResult StateMachineNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );

        // Update transitions
        UpdateTransitionStack( context );

        // If we are fully in a state, update the state directly
        GraphPoseNodeResult result;
        if ( m_pActiveTransition == nullptr )
        {
            result = m_states[m_activeStateIndex].m_pStateNode->Update( context, updateRange );

            // Update node time
            m_duration = m_states[m_activeStateIndex].m_pStateNode->GetDuration();
            m_previousTime = m_states[m_activeStateIndex].m_pStateNode->GetPreviousTime();
            m_currentTime = m_states[m_activeStateIndex].m_pStateNode->GetCurrentTime();
        }
        else // Update the transition
        {
            result = m_pActiveTransition->Update( context, updateRange );

            // Update node time
            m_duration = m_pActiveTransition->GetDuration();
            m_previousTime = m_pActiveTransition->GetPreviousTime();
            m_currentTime = m_pActiveTransition->GetCurrentTime();
        }

        KRG_ASSERT( context.m_sampledEvents.IsValidRange( result.m_sampledEventRange ) );

        // Check for a valid transition, if one exists start it
        if ( context.m_branchState == BranchState::Active )
        {
            EvaluateTransitions( context, result );
        }

        return result;
    }

    void StateMachineNode::DeactivateBranch( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( IsValid() )
        {
            PoseNode::DeactivateBranch( context );

            // Notify transition
            if ( m_pActiveTransition != nullptr )
            {
                m_pActiveTransition->DeactivateBranch( context );
            }
            else // Notify active state
            {
                m_states[m_activeStateIndex].m_pStateNode->DeactivateBranch( context );
            }
        }
    }
}