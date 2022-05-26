#include "Animation_RuntimeGraphNode_Warping.h"
#include "System/Core/Logging/Log.h"
#include "Engine/Animation/AnimationClip.h"
#include "../../Events/AnimationEvent_Warp.h"

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
}

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
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

        //-------------------------------------------------------------------------

        if ( m_pClipReferenceNode->IsValid() )
        {
            auto pAnimation = m_pClipReferenceNode->GetAnimation();
            KRG_ASSERT( pAnimation != nullptr );
         
            // Read warp events
            //-------------------------------------------------------------------------

            for ( auto const pEvent : pAnimation->GetEvents() )
            {
                auto pWarpEvent = Cast<WarpEvent>( pEvent );
                if ( pWarpEvent != nullptr )
                {
                    WarpSection section;
                    section.m_startFrame = pAnimation->GetFrameTime( pWarpEvent->GetStartTime() ).GetLowerBoundFrameIndex();
                    section.m_endFrame = Math::Min( pAnimation->GetNumFrames(), pAnimation->GetFrameTime( pWarpEvent->GetEndTime() ).GetUpperBoundFrameIndex() );
                    section.m_rotationAllowed = pWarpEvent->IsRotationAllowed();
                    section.m_translationAllowed = pWarpEvent->IsTranslationAllowed();

                    KRG_ASSERT( section.m_startFrame < section.m_endFrame );

                    m_warpSections.emplace_back( section );
                }
            }

            // Perform initial warp
            //-------------------------------------------------------------------------

            Percentage const& initialAnimationTime = m_pClipReferenceNode->GetSyncTrack().GetPercentageThrough( initialTime );
            PerformWarp( context, initialAnimationTime );
        }
    }

    void TargetWarpNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        KRG_ASSERT( m_pTargetValueNode != nullptr );
        m_pTargetValueNode->Shutdown( context );
        m_pClipReferenceNode->Shutdown( context );

        m_warpedRootMotion.Clear();
        m_warpSections.clear();

        PoseNode::ShutdownInternal( context );
    }

    void TargetWarpNode::PerformWarp( GraphContext& context, Percentage startTime )
    {
        auto pAnimation = m_pClipReferenceNode->GetAnimation();
        KRG_ASSERT( pAnimation != nullptr );

        m_warpedRootMotion.Clear();

        //-------------------------------------------------------------------------

        if ( m_warpSections.empty() )
        {
            KRG_LOG_ERROR( "Animation", "Tried to warp animation: %s, but there are no warp events set!", pAnimation->GetResourceID().ToString().c_str() );
            return;
        }

        // Read Target
        //-------------------------------------------------------------------------

        Target const warpTarget = m_pTargetValueNode->GetValue<Target>( context );
        if ( !warpTarget.IsTargetSet() )
        {
            KRG_LOG_ERROR( "Animation", "Invalid target detected for warp node!" );
            return;
        }

        if ( warpTarget.IsBoneTarget() )
        {
            KRG_LOG_ERROR( "Animation", "Invalid target detected for warp node!" );
            return;
        }

        m_warpTarget = warpTarget.GetTransform();

        // Perform Warp
        //-------------------------------------------------------------------------

        m_warpedRootMotion = pAnimation->GetRootMotion();
        
        // TODO
    }

    void TargetWarpNode::UpdateWarp( GraphContext& context )
    {
        if ( !GetSettings<TargetWarpNode>()->m_allowTargetUpdate )
        {
            return;
        }

        // Read Target
        //-------------------------------------------------------------------------

        Target const warpTarget = m_pTargetValueNode->GetValue<Target>( context );
        if ( !warpTarget.IsTargetSet() )
        {
            KRG_LOG_ERROR( "Animation", "Invalid target detected for warp node!" );
            return;
        }

        if ( warpTarget.IsBoneTarget() )
        {
            KRG_LOG_ERROR( "Animation", "Invalid target detected for warp node!" );
            return;
        }

        m_warpTarget = warpTarget.GetTransform();

        // Check Target against existing target
        //-------------------------------------------------------------------------

  

        // If the target has changed and we still have warp events, warp the warped track
    }

    Transform TargetWarpNode::CalculateWarpedRootMotionDelta( GraphContext& context ) const
    {
        KRG_ASSERT( m_warpedRootMotion.IsValid() );
        return m_warpedRootMotion.GetDelta( m_pClipReferenceNode->GetPreviousTime(), m_pClipReferenceNode->GetCurrentTime() );
    }

    GraphPoseNodeResult TargetWarpNode::Update( GraphContext& context )
    {
        auto result = m_pClipReferenceNode->Update( context );
        m_duration = m_pClipReferenceNode->GetDuration();
        m_previousTime = m_pClipReferenceNode->GetPreviousTime();
        m_currentTime = m_pClipReferenceNode->GetCurrentTime();

        UpdateWarp( context );
        if ( m_warpedRootMotion.IsValid() )
        {
            result.m_rootMotionDelta = CalculateWarpedRootMotionDelta( context );
        }
        return result;
    }

    GraphPoseNodeResult TargetWarpNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        auto result = m_pClipReferenceNode->Update( context, updateRange );
        m_duration = m_pClipReferenceNode->GetDuration();
        m_previousTime = m_pClipReferenceNode->GetPreviousTime();
        m_currentTime = m_pClipReferenceNode->GetCurrentTime();

        UpdateWarp( context );
        if ( m_warpedRootMotion.IsValid() )
        {
            result.m_rootMotionDelta = CalculateWarpedRootMotionDelta( context );
        }
        return result;
    }
}