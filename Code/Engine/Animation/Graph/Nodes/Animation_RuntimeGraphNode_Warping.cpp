#include "Animation_RuntimeGraphNode_Warping.h"
#include "Engine/Animation/AnimationClip.h"
#include "Engine/Animation/Events/AnimationEvent_Warp.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Drawing/DebugDrawing.h"

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

        auto pSettings = GetSettings<TargetWarpNode>();
        m_samplingMode = pSettings->m_samplingMode;

        //-------------------------------------------------------------------------

        if ( m_pClipReferenceNode->IsValid() )
        {
            auto pAnimation = m_pClipReferenceNode->GetAnimation();
            KRG_ASSERT( pAnimation != nullptr );
         
            m_warpStartTransform = context.m_worldTransform;

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

    bool TargetWarpNode::TryReadTarget( GraphContext& context )
    {
        Target const warpTarget = m_pTargetValueNode->GetValue<Target>( context );
        if ( !warpTarget.IsTargetSet() )
        {
            KRG_LOG_ERROR( "Animation", "Invalid target detected for warp node!" );
            return false;
        }

        if ( warpTarget.IsBoneTarget() )
        {
            KRG_LOG_ERROR( "Animation", "Invalid target detected for warp node!" );
            return false;
        }

        m_warpTarget = warpTarget.GetTransform();
        return true;
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

        if ( !TryReadTarget( context ) )
        {
            return;
        }

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

        // Cannot update an invalid warp
        if ( !m_warpedRootMotion.IsValid() )
        {
            return;
        }

        // Check if the target has changed
        //-------------------------------------------------------------------------

        Transform const previousTarget = m_warpTarget;

        if ( !TryReadTarget( context ) )
        {
            return;
        }

        // TODO: check for significant difference
        if ( m_warpTarget == previousTarget )
        {
            return;
        }

        // Recalculate the warp
        //-------------------------------------------------------------------------

        // TODO
    }

    //-------------------------------------------------------------------------

    GraphPoseNodeResult TargetWarpNode::Update( GraphContext& context )
    {
        MarkNodeActive( context );
        GraphPoseNodeResult result = m_pClipReferenceNode->Update( context );
        UpdateShared( context, result );
        return result;
    }

    GraphPoseNodeResult TargetWarpNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        MarkNodeActive( context );
        GraphPoseNodeResult result = m_pClipReferenceNode->Update( context, updateRange );
        UpdateShared( context, result );
        return result;
    }

    void TargetWarpNode::UpdateShared( GraphContext& context, GraphPoseNodeResult& result )
    {
        auto pSettings = GetSettings<TargetWarpNode>();

        m_duration = m_pClipReferenceNode->GetDuration();
        m_previousTime = m_pClipReferenceNode->GetPreviousTime();
        m_currentTime = m_pClipReferenceNode->GetCurrentTime();

        if ( !m_warpedRootMotion.IsValid() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        UpdateWarp( context );

        // If we are sampling accurately then we need to match the exact world space position each update
        if( m_samplingMode == SamplingMode::Accurate )
        {
            // Calculate error between current and expected position
            Transform const expectedTransform = m_warpedRootMotion.GetTransform( m_previousTime );
            float const positionErrorSq = expectedTransform.GetTranslation().GetDistanceSquared3( context.m_worldTransform.GetTranslation() );
            if( positionErrorSq <= pSettings->m_samplingPositionErrorThresholdSq )
            {
                Transform const desiredFinalTransform = m_warpedRootMotion.GetTransform( m_currentTime );
                result.m_rootMotionDelta = Transform::Delta( context.m_worldTransform, desiredFinalTransform );
            }
            else // Exceeded the error threshold, so fallback to inaccurate sampling
            {
                m_samplingMode = SamplingMode::Inaccurate;
            }
        }

        // Just sample the delta and return that
        if ( m_samplingMode == SamplingMode::Inaccurate )
        {
            result.m_rootMotionDelta = m_warpedRootMotion.GetDelta( m_pClipReferenceNode->GetPreviousTime(), m_pClipReferenceNode->GetCurrentTime() );
        }
    }

    //-------------------------------------------------------------------------

    #if  KRG_DEVELOPMENT_TOOLS
    void TargetWarpNode::DrawDebug( GraphContext& graphContext, Drawing::DrawContext& drawCtx )
    {
        if ( !IsValid() )
        {
            return;
        }

        // Draw Target
        drawCtx.DrawAxis( m_warpTarget, 0.5f, 5.0f );
       
        // Draw Warped Root Motion
        if ( m_warpedRootMotion.IsValid() )
        {
            m_warpedRootMotion.DrawDebug( drawCtx, m_warpStartTransform );
        }
    }
    #endif
}