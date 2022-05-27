#include "AnimationSkeleton.h"
#include "System/Core/Drawing/DebugDrawing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    bool Skeleton::IsValid() const
    {
        return !m_boneIDs.empty() && ( m_boneIDs.size() == m_parentIndices.size() ) && ( m_boneIDs.size() == m_localReferencePose.size() );
    }

    Transform Skeleton::GetBoneGlobalTransform( int32_t idx ) const
    {
        KRG_ASSERT( idx >= 0 && idx < m_localReferencePose.size() );

        Transform boneGlobalTransform = m_localReferencePose[idx];
        int32_t parentIdx = GetParentBoneIndex( idx );

        while ( parentIdx != InvalidIndex )
        {
            boneGlobalTransform = boneGlobalTransform * m_localReferencePose[parentIdx];
            parentIdx = GetParentBoneIndex( parentIdx );
        }

        return boneGlobalTransform;
    }

    int32_t Skeleton::GetFirstChildBoneIndex( int32_t boneIdx ) const
    {
        int32_t const numBones = GetNumBones();
        KRG_ASSERT( IsValidBoneIndex( boneIdx ) );

        int32_t childIdx = InvalidIndex;
        for ( auto i = boneIdx + 1; i < numBones; i++ )
        {
            if ( m_parentIndices[i] == boneIdx )
            {
                childIdx = i;
                break;
            }
        }

        return childIdx;
    }

    bool Skeleton::IsChildBoneOf( int32_t parentBoneIdx, int32_t childBoneIdx ) const
    {
        KRG_ASSERT( IsValidBoneIndex( parentBoneIdx ) );
        KRG_ASSERT( IsValidBoneIndex( childBoneIdx ) );

        bool isChild = false;

        int32_t actualParentBoneIdx = GetParentBoneIndex( childBoneIdx );
        while ( actualParentBoneIdx != InvalidIndex )
        {
            if ( actualParentBoneIdx == parentBoneIdx )
            {
                isChild = true;
                break;
            }

            actualParentBoneIdx = GetParentBoneIndex( actualParentBoneIdx );
        }

        return isChild;
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void Skeleton::DrawDebug( Drawing::DrawContext& ctx, Transform const& worldTransform ) const
    {
        auto const numBones = m_localReferencePose.size();
        if ( numBones > 0 )
        {
            TInlineVector<Transform, 256> globalTransforms;
            globalTransforms.resize( numBones );

            globalTransforms[0] = m_localReferencePose[0] * worldTransform;
            for ( auto i = 1; i < numBones; i++ )
            {
                auto const& parentIdx = m_parentIndices[i];
                auto const& parentTransform = globalTransforms[parentIdx];
                globalTransforms[i] = m_localReferencePose[i] * parentTransform;
            }

            //-------------------------------------------------------------------------

            DrawRootBone( ctx, globalTransforms[0] );

            for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
            {
                auto const& parentIdx = m_parentIndices[boneIdx];
                auto const& parentTransform = globalTransforms[parentIdx];
                auto const& boneTransform = globalTransforms[boneIdx];

                ctx.DrawLine( boneTransform.GetTranslation().ToFloat3(), parentTransform.GetTranslation().ToFloat3(), Colors::HotPink, 2.0f );
                ctx.DrawAxis( boneTransform, 0.03f, 2.0f );
            }
        }
    }

    void DrawRootBone( Drawing::DrawContext& ctx, Transform const& worldTransform )
    {
        constexpr static float const gizmoRadius = 0.045f;
        constexpr static float const arrowLength = 0.1f;

        Vector const fwdDir = worldTransform.GetAxisY().GetNegated();
        Vector const arrowStartPos = Vector::MultiplyAdd( fwdDir, Vector( gizmoRadius ), worldTransform.GetTranslation() );

        ctx.DrawDisc( worldTransform.GetTranslation(), gizmoRadius, Colors::Red );
        ctx.DrawArrow( arrowStartPos, fwdDir, arrowLength, Colors::Lime, 8 );
    }
    #endif
}