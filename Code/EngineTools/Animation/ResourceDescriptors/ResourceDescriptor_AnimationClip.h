#pragma once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Resource/ResourceDescriptor.h"
#include "System/Animation/AnimationSkeleton.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    struct KRG_ENGINETOOLS_API AnimationClipResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( AnimationClipResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return AnimationClip::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE ResourcePath                m_animationPath;
        KRG_EXPOSE TResourcePtr<Skeleton>      m_pSkeleton = nullptr;
        KRG_EXPOSE String                      m_animationName; // Optional: if not set, will use the first animation in the file
        KRG_EXPOSE bool                        m_regenerateRootMotion = false; // Force regeneration of root motion track from the specified bone
        KRG_EXPOSE bool                        m_rootMotionGenerationRestrictToHorizontalPlane = false; // Ensure that the root motion has no vertical motion
        KRG_EXPOSE StringID                    m_rootMotionGenerationBoneID;
        KRG_EXPOSE EulerAngles                 m_rootMotionGenerationPreRotation;
    };
}