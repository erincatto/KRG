#pragma once

#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    // An interface to directly access a selected animation, this is needed to ensure certain animation nodes only operate on animations directly
    class KRG_ENGINE_ANIMATION_API AnimationClipReferenceNode : public PoseNode
    {
    public:

        virtual AnimationClip const* GetAnimation() const = 0;
        virtual void DisableRootMotionSampling() = 0;
        virtual bool IsLooping() const = 0;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationClipNode final : public AnimationClipReferenceNode
    {

    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_playInReverseValueNodeIdx, m_sampleRootMotion, m_allowLooping, m_dataSlotIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t                                   m_playInReverseValueNodeIdx = InvalidIndex;
            bool                                        m_sampleRootMotion = true;
            bool                                        m_allowLooping = false;
            DataSetSlotIndex                            m_dataSlotIdx = InvalidIndex;
        };

    public:

        virtual bool IsValid() const override;

        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;

        virtual AnimationClip const* GetAnimation() const final { KRG_ASSERT( IsValid() ); return m_pAnimation; }
        virtual void DisableRootMotionSampling() final { KRG_ASSERT( IsValid() ); m_shouldSampleRootMotion = false; }
        virtual bool IsLooping() const final { return GetSettings<AnimationClipNode>()->m_allowLooping; }

        virtual SyncTrack const& GetSyncTrack() const override { KRG_ASSERT( IsValid() ); return m_pAnimation->GetSyncTrack(); }

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        GraphPoseNodeResult CalculateResult( GraphContext& context, bool bIsSynchronizedUpdate ) const;

    private:

        AnimationClip const*                            m_pAnimation = nullptr;
        BoolValueNode*                                  m_pPlayInReverseValueNode = nullptr;
        bool                                            m_shouldPlayInReverse = false;
        bool                                            m_shouldSampleRootMotion = true;
    };
}