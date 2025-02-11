#include "Component_AnimationGraph.h"
#include "Engine/Animation/TaskSystem/Animation_TaskSystem.h"
#include "Engine/UpdateContext.h"
#include "Engine/Physics/PhysicsScene.h"
#include "System/Animation/AnimationPose.h"
#include "System/Profiling.h"
#include "System/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphComponent::~AnimationGraphComponent()
    {
        #if KRG_DEVELOPMENT_TOOLS
        KRG_ASSERT( m_pRootMotionActionRecorder == nullptr );
        #endif
    }

    void AnimationGraphComponent::Initialize()
    {
        EntityComponent::Initialize();

        if ( m_pGraphVariation == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pGraphVariation.IsLoaded() );

        #if KRG_DEVELOPMENT_TOOLS
        m_pRootMotionActionRecorder = KRG::New<RootMotionRecorder>();
        #endif

        m_pPose = KRG::New<Pose>( m_pGraphVariation->GetSkeleton() );
        m_pPose->CalculateGlobalTransforms();

        m_pTaskSystem = KRG::New<TaskSystem>( m_pGraphVariation->GetSkeleton() );

        #if KRG_DEVELOPMENT_TOOLS
        m_graphContext.Initialize( GetEntityID().m_ID, m_pTaskSystem, m_pPose, m_pRootMotionActionRecorder );
        #else
        m_graphContext.Initialize( GetEntityID().m_ID, m_pTaskSystem, m_pPose, nullptr );
        #endif

        m_pGraphInstance = KRG::New<GraphInstance>( m_pGraphVariation.GetPtr() );
    }

    void AnimationGraphComponent::Shutdown()
    {
        // If we actually instantiated a graph
        if ( m_pGraphInstance != nullptr )
        {
            if ( m_pGraphInstance->IsInitialized() )
            {
                m_pGraphInstance->Shutdown( m_graphContext );
            }
            m_graphContext.Shutdown();
        }

        #if KRG_DEVELOPMENT_TOOLS
        KRG::Delete( m_pRootMotionActionRecorder );
        #endif

        KRG::Delete( m_pTaskSystem );
        KRG::Delete( m_pGraphInstance );
        KRG::Delete( m_pPose );

        EntityComponent::Shutdown();
    }

    //-------------------------------------------------------------------------

    void AnimationGraphComponent::SetGraphVariation( ResourceID graphResourceID )
    {
        KRG_ASSERT( IsUnloaded() );

        KRG_ASSERT( graphResourceID.IsValid() );
        m_pGraphVariation = graphResourceID;
    }

    //-------------------------------------------------------------------------

    Skeleton const* AnimationGraphComponent::GetSkeleton() const
    {
        return ( m_pGraphVariation != nullptr ) ? m_pGraphVariation->GetSkeleton() : nullptr;
    }

    void AnimationGraphComponent::ResetGraphState()
    {
        if ( m_pGraphInstance != nullptr )
        {
            KRG_ASSERT( m_pGraphInstance->IsInitialized() );
            m_pGraphInstance->Reset( m_graphContext );
        }
        else
        {
            KRG_LOG_ERROR( "Animation", "Trying to reset graph state on a animgraph component that has no state!" );
        }
    }

    void AnimationGraphComponent::EvaluateGraph( Seconds deltaTime, Transform const& characterWorldTransform, Physics::Scene* pPhysicsScene )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        m_graphContext.Update( deltaTime, characterWorldTransform, pPhysicsScene );

        // Notify the root motion recorder we're starting an update
        #if KRG_DEVELOPMENT_TOOLS
        m_pRootMotionActionRecorder->StartCharacterUpdate( characterWorldTransform );
        #endif

        // Initialize graph on the first update
        if ( !m_pGraphInstance->IsInitialized() )
        {
            m_pGraphInstance->Initialize( m_graphContext );
        }

        // Reset last frame's tasks
        m_pTaskSystem->Reset();

        // Update the graph and record the root motion
        GraphPoseNodeResult const result = m_pGraphInstance->UpdateGraph( m_graphContext );
        m_rootMotionDelta = result.m_rootMotionDelta;
    }

    void AnimationGraphComponent::ExecutePrePhysicsTasks( Transform const& characterWorldTransform )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();

        if ( !HasGraph() )
        {
            return;
        }

        // Notify the root motion recorder we're done with the character position update so it can track expected vs actual position
        #if KRG_DEVELOPMENT_TOOLS
        m_pRootMotionActionRecorder->EndCharacterUpdate( characterWorldTransform );
        #endif

        m_pTaskSystem->UpdatePrePhysics( m_graphContext.m_deltaTime, characterWorldTransform, characterWorldTransform.GetInverse() );
    }

    void AnimationGraphComponent::ExecutePostPhysicsTasks()
    {
        KRG_PROFILE_FUNCTION_ANIMATION();

        if ( !HasGraph() )
        {
            return;
        }

        m_pTaskSystem->UpdatePostPhysics( *m_pPose );
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void AnimationGraphComponent::DrawDebug( Drawing::DrawContext& drawingContext )
    {
        if ( !HasGraph() )
        {
            return;
        }

        m_pTaskSystem->DrawDebug( drawingContext );
        m_pGraphInstance->DrawDebug( m_graphContext, drawingContext );
        m_graphContext.GetRootMotionActionRecorder()->DrawDebug( drawingContext );
    }

    void AnimationGraphComponent::SetGraphDebugMode( GraphDebugMode mode )
    {
        if ( m_pGraphInstance != nullptr )
        {
            m_pGraphInstance->SetDebugMode( mode );
        }
        else
        {
            KRG_LOG_ERROR( "Animation", "Trying to set debug state on a animgraph component that has no state!" );
        }
    }

    GraphDebugMode AnimationGraphComponent::GetGraphDebugMode() const
    {
        KRG_ASSERT( HasGraphInstance() );
        return m_pGraphInstance->GetDebugMode();
    }

    void AnimationGraphComponent::SetGraphNodeDebugFilterList( TVector<int16_t> const& filterList )
    {
        if ( m_pGraphInstance != nullptr )
        {
            m_pGraphInstance->SetNodeDebugFilterList( filterList );
        }
        else
        {
            KRG_LOG_ERROR( "Animation", "Trying to set debug state on a animgraph component that has no state!" );
        }
    }

    void AnimationGraphComponent::SetTaskSystemDebugMode( TaskSystemDebugMode mode )
    {
        if ( m_pGraphInstance != nullptr )
        {
            m_pTaskSystem->SetDebugMode( mode );
        }
        else
        {
            KRG_LOG_ERROR( "Animation", "Trying to set debug state on a animgraph component that has no state!" );
        }
    }

    TaskSystemDebugMode AnimationGraphComponent::GetTaskSystemDebugMode() const
    {
        KRG_ASSERT( HasGraphInstance() );
        return m_pTaskSystem->GetDebugMode();
    }

    void AnimationGraphComponent::SetRootMotionDebugMode( RootMotionRecorderDebugMode mode )
    {
        if ( m_pGraphInstance != nullptr )
        {
            m_graphContext.GetRootMotionActionRecorder()->SetDebugMode( mode );
        }
        else
        {
            KRG_LOG_ERROR( "Animation", "Trying to set debug state on a animgraph component that has no state!" );
        }
    }

    RootMotionRecorderDebugMode AnimationGraphComponent::GetRootMotionDebugMode() const
    {
        KRG_ASSERT( HasGraphInstance() );
        return m_graphContext.GetRootMotionActionRecorder()->GetDebugMode();
    }
    #endif
}