#include "AnimationTarget.h"
#include "System/Animation/AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    bool Target::TryGetTransform( Pose const* pPose, Transform& outTransform ) const
    {
        KRG_ASSERT( m_isSet );

        //-------------------------------------------------------------------------

        bool const isBoneTarget = IsBoneTarget();
        if ( isBoneTarget )
        {
            KRG_ASSERT( pPose != nullptr );
            auto pSkeleton = pPose->GetSkeleton();

            int32_t const boneIdx = pSkeleton->GetBoneIndex( m_boneID );
            if ( boneIdx != InvalidIndex )
            {
                if ( m_hasOffsets )
                {
                    Transform parentTransform = Transform::Identity;

                    // Get the local transform and the parent global transform
                    if ( m_isUsingBoneSpaceOffsets )
                    {
                        int32_t const parentBoneIdx = pSkeleton->GetParentBoneIndex( m_boneID );
                        if ( parentBoneIdx != InvalidIndex )
                        {
                            parentTransform = pPose->GetGlobalTransform( parentBoneIdx );
                        }

                        outTransform = pPose->GetTransform( boneIdx );
                    }
                    else // Get the global transform for the target bone
                    {
                        outTransform = pPose->GetGlobalTransform( boneIdx );
                    }

                    //-------------------------------------------------------------------------

                    outTransform.SetRotation( outTransform.GetRotation() * m_transform.GetRotation() );
                    outTransform.SetTranslation( outTransform.GetTranslation() + m_transform.GetTranslation() );

                    if ( m_isUsingBoneSpaceOffsets && isBoneTarget )
                    {
                        outTransform *= parentTransform;
                    }
                }
                else
                {
                    outTransform = pPose->GetGlobalTransform( boneIdx );
                }
            }
            else
            {
                return false;
            }
        }
        else // Just use the internal transform
        {
            outTransform = m_transform;
        }

        return true;
    }
}