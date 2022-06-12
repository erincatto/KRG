#include "Component_AnimationClipPlayer.h"
#include "System/Animation/AnimationPose.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void AnimationClipPlayerComponent::SetAnimation( ResourceID animationResourceID )
    {
        KRG_ASSERT( IsUnloaded() );
        KRG_ASSERT( animationResourceID.IsValid() );
        m_pAnimation = animationResourceID;
    }

    void AnimationClipPlayerComponent::SetPlayMode( PlayMode mode )
    {
        m_playMode = mode;
        m_previousAnimTime = Percentage( -1 );
    }

    void AnimationClipPlayerComponent::SetAnimTime( Percentage inTime )
    {
        m_animTime = inTime.GetClamped( m_playMode == PlayMode::Loop );
    }

    void AnimationClipPlayerComponent::SetAnimTime( Seconds inTime )
    {
        if ( !IsInitialized() )
        {
            KRG_LOG_ERROR( "Animation", "Anim Clip Player - Try to set anim time on an uninitialized component!" );
            return;
        }

        if ( m_pAnimation == nullptr )
        {
            KRG_LOG_ERROR( "Animation", "Anim Clip Player - Try to set anim time on a player with no animation set!" );
            return;
        }

        KRG_ASSERT( m_pAnimation.IsLoaded() );
        Percentage const percentage( inTime / m_pAnimation->GetDuration() );
        SetAnimTime( percentage );
    }

    //-------------------------------------------------------------------------

    void AnimationClipPlayerComponent::Initialize()
    {
        EntityComponent::Initialize();

        if ( m_pAnimation != nullptr )
        {
            KRG_ASSERT( m_pAnimation.IsLoaded() );
            m_pPose = KRG::New<Pose>( m_pAnimation->GetSkeleton() );
        }
    }

    void AnimationClipPlayerComponent::Shutdown()
    {
        KRG::Delete( m_pPose );
        m_previousAnimTime = -1.0f;
        EntityComponent::Shutdown();
    }

    //-------------------------------------------------------------------------

    Skeleton const* AnimationClipPlayerComponent::GetSkeleton() const
    {
        return ( m_pAnimation != nullptr ) ? m_pAnimation->GetSkeleton() : nullptr;
    }

    void AnimationClipPlayerComponent::Update( Seconds deltaTime, Transform const& characterTransform )
    {
        KRG_PROFILE_FUNCTION_ANIMATION();
        
        if ( m_pAnimation == nullptr )
        {
            return;
        }

        KRG_ASSERT( m_pPose != nullptr );

        //-------------------------------------------------------------------------

        bool bShouldUpdate = false;

        switch ( m_playMode )
        {
            case PlayMode::Loop:
            {
                m_previousAnimTime = m_animTime;
                m_animTime += Percentage( deltaTime / m_pAnimation->GetDuration() );
                m_animTime = m_animTime.GetClamped( true );
                bShouldUpdate = true;
            }
            break;

            case PlayMode::PlayOnce:
            {
                if ( m_previousAnimTime < 1.0f )
                {
                    m_previousAnimTime = m_animTime;
                    m_animTime += Percentage( deltaTime / m_pAnimation->GetDuration() );
                    m_animTime = m_animTime.GetClamped( false );
                    bShouldUpdate = true;
                }
            }
            break;

            case PlayMode::Posed:
            {
                if ( m_previousAnimTime != m_animTime )
                {
                    m_previousAnimTime = m_animTime;
                    bShouldUpdate = true;
                }
            }
            break;
        }

        //-------------------------------------------------------------------------

        if ( bShouldUpdate )
        {
            m_pAnimation->GetPose( m_animTime, m_pPose );
            m_pPose->CalculateGlobalTransforms();
            m_rootMotionDelta = m_pAnimation->GetRootMotionDelta( m_previousAnimTime, m_animTime );
        }
        else // Clear the root motion delta
        {
            m_rootMotionDelta = Transform::Identity;
        }
    }
}