#pragma once

#include "Animation_RuntimeGraphNode_AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class KRG_ENGINE_ANIMATION_API OrientationWarpNode final : public PoseNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_clipReferenceNodeIdx, m_angleOffsetValueNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex          m_clipReferenceNodeIdx = InvalidIndex;
            GraphNodeIndex          m_angleOffsetValueNodeIdx = InvalidIndex;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return m_pClipReferenceNode->GetSyncTrack(); }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        void PerformWarp( GraphContext& context );
        Transform SampleWarpedRootMotion( GraphContext& context ) const;

    private:

        AnimationClipReferenceNode*     m_pClipReferenceNode = nullptr;
        FloatValueNode*                 m_pAngleOffsetValueNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API TargetWarpNode final : public PoseNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_clipReferenceNodeIdx, m_targetValueNodeIdx, m_clampMaxUpdateAngleDelta, m_clampMaxUpdatePositionDelta, m_allowTargetUpdate );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex              m_clipReferenceNodeIdx = InvalidIndex;
            GraphNodeIndex              m_targetValueNodeIdx = InvalidIndex;
            float                       m_clampMaxUpdateAngleDelta = 0.0f;
            float                       m_clampMaxUpdatePositionDelta = 0.0f;
            bool                        m_allowTargetUpdate = false;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return m_pClipReferenceNode->GetSyncTrack(); }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        void PerformWarp( GraphContext& context );
        void UpdateWarp( GraphContext& context );
        Transform SampleWarpedRootMotion( GraphContext& context ) const;

    private:

        AnimationClipReferenceNode*     m_pClipReferenceNode = nullptr;
        TargetValueNode*                m_pTargetValueNode = nullptr;
    };
}