#include "Animation_RuntimeGraphNode_Warping.h"
#include "System/Core/Logging/Log.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void OrientationWarpNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<OrientationWarpNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_clipReferenceNodeIdx, pNode->m_pClipReferenceNode );
        SetNodePtrFromIndex( nodePtrs, m_angleOffsetValueNodeIdx, pNode->m_pAngleOffsetValueNode );
    }

    void OrientationWarpNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_pAngleOffsetValueNode != nullptr );
        PoseNode::InitializeInternal( context, initialTime );
        m_pClipReferenceNode->Initialize( context, initialTime );
        m_pAngleOffsetValueNode->Initialize( context );

        //-------------------------------------------------------------------------

        PerformWarp( context );
    }

    void OrientationWarpNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_pAngleOffsetValueNode != nullptr );
        m_pAngleOffsetValueNode->Shutdown( context );
        m_pClipReferenceNode->Shutdown( context );
        PoseNode::ShutdownInternal( context );
    }

    void OrientationWarpNode::PerformWarp( GraphContext& context )
    {
        auto pAnimation = m_pClipReferenceNode->GetAnimation();
        KRG_ASSERT( pAnimation != nullptr );
    }

    Transform OrientationWarpNode::SampleWarpedRootMotion( GraphContext& context ) const
    {
        return Transform();
    }

    GraphPoseNodeResult OrientationWarpNode::Update( GraphContext& context )
    {
        auto result = m_pClipReferenceNode->Update( context );
        m_duration = m_pClipReferenceNode->GetDuration();
        m_previousTime = m_pClipReferenceNode->GetPreviousTime();
        m_currentTime = m_pClipReferenceNode->GetCurrentTime();

        result.m_rootMotionDelta = SampleWarpedRootMotion( context );
        return result;
    }

    GraphPoseNodeResult OrientationWarpNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        auto result = m_pClipReferenceNode->Update( context, updateRange );
        m_duration = m_pClipReferenceNode->GetDuration();
        m_previousTime = m_pClipReferenceNode->GetPreviousTime();
        m_currentTime = m_pClipReferenceNode->GetCurrentTime();

        result.m_rootMotionDelta = SampleWarpedRootMotion( context );
        return result;
    }

    //-------------------------------------------------------------------------

    void TargetWarpNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<TargetWarpNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_clipReferenceNodeIdx, pNode->m_pClipReferenceNode );
        SetNodePtrFromIndex( nodePtrs, m_targetValueNodeIdx, pNode->m_pTargetValueNode );
    }

    void TargetWarpNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_pTargetValueNode != nullptr );
        PoseNode::InitializeInternal( context, initialTime );
        m_pClipReferenceNode->Initialize( context, initialTime );
        m_pTargetValueNode->Initialize( context );
    }

    void TargetWarpNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_pTargetValueNode != nullptr );
        m_pTargetValueNode->Shutdown( context );
        m_pClipReferenceNode->Shutdown( context );
        PoseNode::ShutdownInternal( context );
    }

    void TargetWarpNode::PerformWarp( GraphContext& context )
    {
        auto pAnimation = m_pClipReferenceNode->GetAnimation();
        KRG_ASSERT( pAnimation != nullptr );
    }

    void TargetWarpNode::UpdateWarp( GraphContext& context )
    {

    }

    Transform TargetWarpNode::SampleWarpedRootMotion( GraphContext& context ) const
    {
        return Transform();
    }

    GraphPoseNodeResult TargetWarpNode::Update( GraphContext& context )
    {
        auto result = m_pClipReferenceNode->Update( context );
        m_duration = m_pClipReferenceNode->GetDuration();
        m_previousTime = m_pClipReferenceNode->GetPreviousTime();
        m_currentTime = m_pClipReferenceNode->GetCurrentTime();

        UpdateWarp( context );
        result.m_rootMotionDelta = SampleWarpedRootMotion( context );
        return result;
    }

    GraphPoseNodeResult TargetWarpNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        auto result = m_pClipReferenceNode->Update( context, updateRange );
        m_duration = m_pClipReferenceNode->GetDuration();
        m_previousTime = m_pClipReferenceNode->GetPreviousTime();
        m_currentTime = m_pClipReferenceNode->GetCurrentTime();

        Percentage const startTime = m_pClipReferenceNode->GetPreviousTime();
        Percentage const endTime = m_pClipReferenceNode->GetCurrentTime();
        result.m_rootMotionDelta = SampleWarpedRootMotion( context );
        return result;
    }
}