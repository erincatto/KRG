#pragma once 

#include "System/_Module/API.h"
#include "AnimationSkeleton.h"
#include "System/Math/Math.h"
#include "System/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG::Drawing { class DrawContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_SYSTEM_API Pose
    {
        friend class Blender;
        friend class AnimationClip;

    public:

        enum class Type
        {
            None,
            ReferencePose,
            ZeroPose
        };

        enum class State
        {
            Unset,
            Pose,
            ReferencePose,
            ZeroPose,
            AdditivePose
        };

    public:

        Pose( Skeleton const* pSkeleton, Type initialPoseType = Type::ReferencePose );

        // Move
        Pose( Pose&& rhs );
        Pose& operator=( Pose&& rhs );

        // Explicitly disable the copy operation to prevent accidental copies
        Pose( Pose const& rhs ) = delete;
        Pose& operator=( Pose const& rhs ) = delete;

        void CopyFrom( Pose const& rhs );
        KRG_FORCE_INLINE void CopyFrom( Pose const* pRhs ) { CopyFrom( *pRhs ); }

        //-------------------------------------------------------------------------

        inline int32_t GetNumBones() const { return m_pSkeleton->GetNumBones(); }
        inline Skeleton const* GetSkeleton() const { return m_pSkeleton; }

        // Pose state
        //-------------------------------------------------------------------------

        void Reset( Type initState = Type::None, bool calcGlobalPose = false );

        inline bool IsPoseSet() const { return m_state != State::Unset; }
        inline bool IsReferencePose() const { return m_state == State::ReferencePose; }
        inline bool IsZeroPose() const { return m_state == State::ZeroPose; }
        inline bool IsAdditivePose() const { return m_state == State::AdditivePose; }

        // Local Transforms
        //-------------------------------------------------------------------------

        TVector<Transform> const& GetTransforms() const { return m_localTransforms; }

        inline Transform const& GetTransform( int32_t boneIdx ) const
        {
            KRG_ASSERT( boneIdx < GetNumBones() );
            return m_localTransforms[boneIdx];
        }

        inline void SetTransform( int32_t boneIdx, Transform const& transform )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx] = transform;
            MarkAsValidPose();
        }

        inline void SetRotation( int32_t boneIdx, Quaternion const& rotation )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetRotation( rotation );
            MarkAsValidPose();
        }

        inline void SetTranslation( int32_t boneIdx, Float3 const& translation )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetTranslation( translation );
            MarkAsValidPose();
        }

        // Set the scale for a given bone, note will change pose state to "Pose" if not already set
        inline void SetScale( int32_t boneIdx, Float3 const& scale )
        {
            KRG_ASSERT( boneIdx < GetNumBones() && boneIdx >= 0 );
            m_localTransforms[boneIdx].SetScale( scale );
            MarkAsValidPose();
        }

        // Global Transform Cache
        //-------------------------------------------------------------------------

        inline bool HasGlobalTransforms() const { return !m_globalTransforms.empty(); }
        inline void ClearGlobalTransforms() { m_globalTransforms.clear(); }
        inline TVector<Transform> const& GetGlobalTransforms() const { return m_globalTransforms; }
        void CalculateGlobalTransforms();
        Transform GetGlobalTransform( int32_t boneIdx ) const;

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void DrawDebug( Drawing::DrawContext& ctx, Transform const& worldTransform, Color color = Colors::HotPink ) const;
        #endif

    private:

        Pose() = delete;

        void SetToReferencePose( bool setGlobalPose );
        void SetToZeroPose( bool setGlobalPose );

        KRG_FORCE_INLINE void MarkAsValidPose()
        {
            if ( m_state != State::Pose && m_state != State::AdditivePose )
            {
                m_state = State::Pose;
            }
        }

    private:

        Skeleton const*             m_pSkeleton;                // The skeleton for this pose
        TVector<Transform>          m_localTransforms;          // Parent-space transforms
        TVector<Transform>          m_globalTransforms;         // Character-space transforms
        State                       m_state = State::Unset;     // Pose state
    };
}