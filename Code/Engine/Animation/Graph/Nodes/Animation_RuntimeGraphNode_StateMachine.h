#pragma once

#include "Animation_RuntimeGraphNode_Transition.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class KRG_ENGINE_API StateMachineNode final : public PoseNode
    {

    public:

        using StateIndex = int16_t;

        struct TransitionSettings
        {
            KRG_SERIALIZE( m_targetStateIdx, m_transitionNodeIdx, m_conditionNodeIdx );

            StateIndex                                              m_targetStateIdx = InvalidIndex;
            int16_t                                          m_conditionNodeIdx = InvalidIndex;
            int16_t                                          m_transitionNodeIdx = InvalidIndex;
        };

        struct StateSettings
        {
            KRG_SERIALIZE( m_stateNodeIdx, m_entryConditionNodeIdx, m_transitionSettings );

            int16_t                                          m_stateNodeIdx = InvalidIndex;
            int16_t                                          m_entryConditionNodeIdx = InvalidIndex;
            TInlineVector<TransitionSettings, 5>                    m_transitionSettings;
        };

        //-------------------------------------------------------------------------

        struct KRG_ENGINE_API Settings : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_stateSettings, m_defaultStateIndex );

        public:

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

        public:

            TInlineVector<StateSettings, 5>                         m_stateSettings;
            StateIndex                                              m_defaultStateIndex = InvalidIndex;
        };

        //-------------------------------------------------------------------------

        struct TransitionInfo
        {
            TransitionNode*                                         m_pTransitionNode = nullptr;
            BoolValueNode*                                          m_pConditionNode = nullptr;
            StateIndex                                              m_targetStateIdx = InvalidIndex;
        };

        struct StateInfo
        {
            bool HasForceableTransitions() const;

            StateNode*                                              m_pStateNode = nullptr;
            BoolValueNode*                                          m_pEntryConditionNode = nullptr;
            TInlineVector<TransitionInfo, 5>                        m_transitions;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override;
        virtual bool IsValid() const override;

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

        StateIndex SelectDefaultState( GraphContext& context ) const;

        void InitializeTransitionConditions( GraphContext& context );
        void ShutdownTransitionConditions( GraphContext& context );

        void EvaluateTransitions( GraphContext& context, GraphPoseNodeResult& NodeResult );
        void UpdateTransitionStack( GraphContext& context );

    private:

        TInlineVector<StateInfo, 10>                                m_states;
        TransitionNode*                                             m_pActiveTransition = nullptr;
        StateIndex                                                  m_activeStateIndex = InvalidIndex;
    };
}