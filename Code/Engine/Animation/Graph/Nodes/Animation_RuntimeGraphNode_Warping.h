#pragma once

#include "Animation_RuntimeGraphNode_AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Warping
{

}

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

            int16_t                     m_clipReferenceNodeIdx = InvalidIndex;
            int16_t                     m_angleOffsetValueNodeIdx = InvalidIndex;
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
        struct WarpSection
        {
            int32_t                     m_startFrame = 0;
            int32_t                     m_endFrame = 0;
            bool                        m_rotationAllowed = false;
            bool                        m_translationAllowed = false;
        };

    public:

        enum class SamplingMode
        {
            KRG_REGISTER_ENUM

            Inaccurate = 0, // Just returns the delta for each update from the warped root motion
            Accurate,       // Will return a delta that attempts to move the character to the expected world space position
        };

        struct KRG_ENGINE_ANIMATION_API Settings final : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_clipReferenceNodeIdx, m_targetValueNodeIdx, m_samplingPositionErrorThresholdSq, m_samplingMode, m_allowTargetUpdate );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t                     m_clipReferenceNodeIdx = InvalidIndex;
            int16_t                     m_targetValueNodeIdx = InvalidIndex;
            float                       m_samplingPositionErrorThresholdSq = 0.0f; // The threshold at which we switch from accurate to inaccurate sampling
            SamplingMode                m_samplingMode = SamplingMode::Inaccurate;
            bool                        m_allowTargetUpdate = false;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return m_pClipReferenceNode->GetSyncTrack(); }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        void UpdateShared( GraphContext& context, GraphPoseNodeResult& result );

        bool TryReadTarget( GraphContext& context );
        void PerformWarp( GraphContext& context, Percentage startTime );
        void UpdateWarp( GraphContext& context );

        #if KRG_DEVELOPMENT_TOOLS
        virtual void DrawDebug( GraphContext& graphContext, Drawing::DrawContext& drawCtx ) override;
        #endif

    private:

        AnimationClipReferenceNode*     m_pClipReferenceNode = nullptr;
        TargetValueNode*                m_pTargetValueNode = nullptr;
        Transform                       m_warpTarget;
        Transform                       m_warpStartTransform;
        RootMotionData                  m_warpedRootMotion;
        TInlineVector<WarpSection, 3>   m_warpSections;
        SamplingMode                    m_samplingMode;
    };
}