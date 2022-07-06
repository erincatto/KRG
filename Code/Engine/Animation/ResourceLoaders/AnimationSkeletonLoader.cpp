#include "AnimationSkeletonLoader.h"
#include "System/Animation/AnimationSkeleton.h"
#include "System/Serialization/BinarySerialization.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    SkeletonLoader::SkeletonLoader()
    {
        m_loadableTypes.push_back( Skeleton::GetStaticResourceTypeID() );
    }

    bool SkeletonLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryInputArchive& archive ) const
    {
        Skeleton* pSkeleton = KRG::New<Skeleton>();
        archive << *pSkeleton;
        KRG_ASSERT( pSkeleton->IsValid() );
        pResourceRecord->SetResourceData( pSkeleton );

        // Calculate global reference pose
        //-------------------------------------------------------------------------

        int32_t const numBones = pSkeleton->GetNumBones();
        pSkeleton->m_globalReferencePose.resize( numBones );

        pSkeleton->m_globalReferencePose[0] = pSkeleton->m_localReferencePose[0];
        for ( auto boneIdx = 1; boneIdx < numBones; boneIdx++ )
        {
            int32_t const parentIdx = pSkeleton->GetParentBoneIndex( boneIdx );
            pSkeleton->m_globalReferencePose[boneIdx] = pSkeleton->m_localReferencePose[boneIdx] * pSkeleton->m_globalReferencePose[parentIdx];
        }

        //-------------------------------------------------------------------------

        return true;
    }
}