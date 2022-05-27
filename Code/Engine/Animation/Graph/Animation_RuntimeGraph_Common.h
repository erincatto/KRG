#pragma once

#include "Engine/Animation/_Module/API.h"
#include "Engine/Animation/AnimationCommon.h"
#include "System/Core/Types/Color.h"
#include "System/Core/KRG.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using GraphNodeIndex = int16_t;

    //-------------------------------------------------------------------------

    enum class GraphValueType
    {
        KRG_REGISTER_ENUM

        Unknown = 0,
        Bool,
        ID,
        Int,
        Float,
        Vector,
        Target,
        BoneMask,
        Pose
    };

    #if KRG_DEVELOPMENT_TOOLS
    KRG_ENGINE_ANIMATION_API Color GetColorForValueType( GraphValueType type );
    KRG_ENGINE_ANIMATION_API char const* GetNameForValueType( GraphValueType type );
    #endif

    //-------------------------------------------------------------------------

    // Used to signify if a node or node output is coming from an active state (i.e. a state we are not transitioning away from)
    enum class BranchState
    {
        Active,
        Inactive,
    };
}