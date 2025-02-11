#include "Animation_TaskSystem.h"
#include "Tasks/Animation_Task_DefaultPose.h"
#include "Engine/Animation/AnimationBlender.h"
#include "System/Log.h"
#include "System/Drawing/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    TaskSystem::TaskSystem( Skeleton const* pSkeleton )
        : m_posePool( pSkeleton )
        , m_taskContext( m_posePool )
    {
        KRG_ASSERT( pSkeleton != nullptr );
    }

    TaskSystem::~TaskSystem()
    {
        Reset();
    }

    void TaskSystem::Reset()
    {
        for ( auto pTask : m_tasks )
        {
            KRG::Delete( pTask );
        }

        m_tasks.clear();
        m_posePool.Reset();
        m_hasPhysicsDependency = false;
    }

    //-------------------------------------------------------------------------

    void TaskSystem::RollbackToTaskIndexMarker( TaskIndex const marker )
    {
        KRG_ASSERT( marker >= 0 && marker <= m_tasks.size() );

        for ( int16_t t = (int16_t) m_tasks.size() - 1; t >= marker; t-- )
        {
            KRG::Delete( m_tasks[t] );
            m_tasks.erase( m_tasks.begin() + t );
        }
    }

    //-------------------------------------------------------------------------

    bool TaskSystem::AddTaskChainToPrePhysicsList( TaskIndex taskIdx )
    {
        KRG_ASSERT( taskIdx >= 0 && taskIdx < m_tasks.size() );
        KRG_ASSERT( m_hasCodependentPhysicsTasks == false );

        auto pTask = m_tasks[taskIdx];
        for ( auto DepTaskIdx : pTask->GetDependencyIndices() )
        {
            if ( !AddTaskChainToPrePhysicsList( DepTaskIdx ) )
            {
                return false;
            }
        }

        //-------------------------------------------------------------------------

        // Cant have a dependency that relies on physics
        if ( pTask->GetRequiredUpdateStage() == TaskUpdateStage::PostPhysics )
        {
            return false;
        }

        // Cant add the same task twice, i.e. codependency
        if ( VectorContains( m_prePhysicsTaskIndices, taskIdx ) )
        {
            return false;
        }

        m_prePhysicsTaskIndices.emplace_back( taskIdx );
        return true;
    }

    void TaskSystem::UpdatePrePhysics( float deltaTime, Transform const& worldTransform, Transform const& worldTransformInverse )
    {
        m_taskContext.m_deltaTime = deltaTime;
        m_taskContext.m_worldTransform = worldTransform;
        m_taskContext.m_worldTransformInverse = worldTransformInverse;
        m_taskContext.m_updateStage = TaskUpdateStage::PrePhysics;

        m_prePhysicsTaskIndices.clear();
        m_hasCodependentPhysicsTasks = false;

        // Conditionally execute all pre-physics tasks
        //-------------------------------------------------------------------------

        if ( m_hasPhysicsDependency )
        {
            // Go backwards through the registered task and execute all task chains with a pre-physics requirement
            auto const numTasks = (int8_t) m_tasks.size();
            for ( TaskIndex i = 0; i < numTasks; i++ )
            {
                if ( m_tasks[i]->GetRequiredUpdateStage() == TaskUpdateStage::PrePhysics )
                {
                    if ( !AddTaskChainToPrePhysicsList( i ) )
                    {
                        m_hasCodependentPhysicsTasks = true;
                        break;
                    }
                }
            }

            // If we've detected a co-dependent physics task, ignore all registered tasks by just pushing a new task and immediately executing it
            if ( m_hasCodependentPhysicsTasks )
            {
                KRG_LOG_WARNING( "Animation", "Co-dependent physics tasks detected!" );
                RegisterTask<Tasks::DefaultPoseTask>( (int16_t) InvalidIndex, Pose::Type::ReferencePose );
                m_tasks.back()->Execute( m_taskContext );
            }
            else // Execute pre-physics tasks
            {
                for ( TaskIndex prePhysicsTaskIdx : m_prePhysicsTaskIndices )
                {
                    // Set dependencies
                    m_taskContext.m_dependencies.clear();
                    for ( auto depTaskIdx : m_tasks[prePhysicsTaskIdx]->GetDependencyIndices() )
                    {
                        KRG_ASSERT( m_tasks[depTaskIdx]->IsComplete() );
                        m_taskContext.m_dependencies.emplace_back( m_tasks[depTaskIdx] );
                    }

                    m_tasks[prePhysicsTaskIdx]->Execute( m_taskContext );
                }
            }
        }
        else // If we have no physics dependent tasks, execute all tasks now
        {
            ExecuteTasks();
        }
    }

    void TaskSystem::UpdatePostPhysics( Pose& outPose )
    {
        m_taskContext.m_updateStage = TaskUpdateStage::PostPhysics;

        // If we detected co-dependent tasks in the pre-physics update, there's nothing to do here
        if ( m_hasCodependentPhysicsTasks )
        {
            return;
        }

        // Execute tasks
        //-------------------------------------------------------------------------
        // Only run tasks if we have a physics dependency, else all tasks were already executed in the first update stage

        if ( m_hasPhysicsDependency )
        {
            ExecuteTasks();
        }

        // Reflect animation pose out
        //-------------------------------------------------------------------------

        if ( !m_tasks.empty() )
        {
            auto pFinalTask = m_tasks.back();
            KRG_ASSERT( pFinalTask->IsComplete() );
            PoseBuffer const* pResultPoseBuffer = m_posePool.GetBuffer( pFinalTask->GetResultBufferIndex() );
            Pose const* pResultPose = &pResultPoseBuffer->m_pose;

            // Always return a non-additive pose
            if ( pResultPose->IsAdditivePose() )
            {
                outPose.Reset( Pose::Type::ReferencePose );
                TBitFlags<PoseBlendOptions> blendOptions( PoseBlendOptions::Additive );
                Blender::Blend( &outPose, pResultPose, 1.0f, blendOptions, nullptr, &outPose );
            }
            else // Just copy the pose
            {
                outPose.CopyFrom( pResultPoseBuffer->m_pose );
            }

            // Calculate the global transforms and release the task pose buffer
            outPose.CalculateGlobalTransforms();
            m_posePool.ReleasePoseBuffer( pFinalTask->GetResultBufferIndex() );
        }
        else
        {
            outPose.Reset( Pose::Type::ReferencePose, true );
        }
    }

    void TaskSystem::ExecuteTasks()
    {
        int16_t const numTasks = (int8_t) m_tasks.size();
        for ( TaskIndex i = 0; i < numTasks; i++ )
        {
            if ( !m_tasks[i]->IsComplete() )
            {
                // Set dependencies
                m_taskContext.m_dependencies.clear();
                for ( auto DepTaskIdx : m_tasks[i]->GetDependencyIndices() )
                {
                    KRG_ASSERT( m_tasks[DepTaskIdx]->IsComplete() );
                    m_taskContext.m_dependencies.emplace_back( m_tasks[DepTaskIdx] );
                }

                // Execute task
                m_tasks[i]->Execute( m_taskContext );
            }
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void TaskSystem::SetDebugMode( TaskSystemDebugMode mode )
    {
        m_debugMode = mode;
        m_posePool.EnableRecording( m_debugMode != TaskSystemDebugMode::Off );
    }

    void TaskSystem::CalculateTaskOffset( TaskIndex taskIdx, Float2 const& currentOffset, TInlineVector<Float2, 16>& offsets )
    {
        KRG_ASSERT( taskIdx >= 0 && taskIdx < m_tasks.size() );
        auto pTask = m_tasks[taskIdx];
        offsets[taskIdx] = currentOffset;
        
        int32_t const numDependencies = pTask->GetNumDependencies();
        if ( numDependencies == 0 )
        {
            // Do nothing
        }
        else if ( numDependencies == 1 )
        {
            Float2 childOffset = currentOffset;
            childOffset += Float2( 0, -1 );
            CalculateTaskOffset( pTask->GetDependencyIndices()[0], childOffset, offsets );
        }
        else // multiple dependencies
        {
            Float2 childOffset = currentOffset;
            childOffset += Float2( 0, -1 );

            float const childTaskStartOffset = -( numDependencies - 1.0f ) / 2.0f;
            for ( int32_t i = 0; i < numDependencies; i++ )
            {
                childOffset.m_x = currentOffset.m_x + childTaskStartOffset + i;
                CalculateTaskOffset( pTask->GetDependencyIndices()[i], childOffset, offsets );
            }
        }
    }

    void TaskSystem::DrawDebug( Drawing::DrawContext& drawingContext )
    {
        if ( m_debugMode == TaskSystemDebugMode::Off )
        {
            return;
        }

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_posePool.IsRecordingEnabled() );
        if ( !HasTasks() || !m_posePool.HasRecordedData() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_debugMode == TaskSystemDebugMode::FinalPose )
        {
            auto const& pFinalTask = m_tasks.back();
            KRG_ASSERT( pFinalTask->IsComplete() );
            auto pPoseBuffer = m_posePool.GetRecordedPose( (int8_t) m_tasks.size() - 1 );
            pPoseBuffer->m_pose.DrawDebug( drawingContext, m_taskContext.m_worldTransform );
            return;
        }

        // Calculate task tree offsets
        //-------------------------------------------------------------------------

        TInlineVector<Float2, 16> taskTreeOffsets;
        taskTreeOffsets.resize( m_tasks.size(), Float2::Zero );
        CalculateTaskOffset( (TaskIndex) m_tasks.size() - 1, Float2::Zero, taskTreeOffsets );

        Vector const offsetVectorX = m_taskContext.m_worldTransform.GetAxisX().GetNegated() * 2.0f;
        Vector const offsetVectorY = m_taskContext.m_worldTransform.GetAxisY().GetNegated() * 1.5f;

        TInlineVector<Transform, 16> taskTransforms;
        taskTransforms.resize( m_tasks.size(), m_taskContext.m_worldTransform );

        for ( int8_t i = (int8_t) m_tasks.size() - 1; i >= 0; i-- )
        {
            Vector const offset = ( offsetVectorX * taskTreeOffsets[i].m_x ) + ( offsetVectorY * taskTreeOffsets[i].m_y );
            taskTransforms[i].SetTranslation( m_taskContext.m_worldTransform.GetTranslation() + offset );
        }

        // Draw tree
        //-------------------------------------------------------------------------

        for ( int8_t i = (int8_t) m_tasks.size() - 1; i >= 0; i-- )
        {
            auto pPoseBuffer = m_posePool.GetRecordedPose( i );
            pPoseBuffer->m_pose.DrawDebug( drawingContext, taskTransforms[i], m_tasks[i]->GetDebugColor() );
            drawingContext.DrawText3D( taskTransforms[i].GetTranslation(), m_tasks[i]->GetDebugText().c_str(), m_tasks[i]->GetDebugColor(), Drawing::FontSmall, Drawing::AlignMiddleCenter );

            for ( auto& dependencyIdx : m_tasks[i]->GetDependencyIndices() )
            {
                drawingContext.DrawLine( taskTransforms[i].GetTranslation(), taskTransforms[dependencyIdx].GetTranslation(), m_tasks[i]->GetDebugColor(), 2.0f );
            }
        }
    }
    #endif
}