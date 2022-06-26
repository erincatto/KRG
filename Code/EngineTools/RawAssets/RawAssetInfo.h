#pragma once

#include "../_Module/API.h"
#include "System/Types/Containers.h"
#include "System/Types/String.h"
#include "System/Types/StringID.h"
#include "System/Time/Time.h"
#include "System/Math/Math.h"

//-------------------------------------------------------------------------
// Provide info about a raw file
//-------------------------------------------------------------------------
// These are various info data structures allowing our tools to get information regarding the contents of a raw file

namespace KRG::RawAssets
{
    struct KRG_ENGINETOOLS_API RawMeshInfo
    {
        String      m_name;
        String      m_materialName;
        bool        m_isSkinned = false;
    };

    struct KRG_ENGINETOOLS_API RawAnimationInfo
    {
        String      m_name;
        Seconds     m_duration = 0;
        float       m_frameRate = 0;
    };

    struct KRG_ENGINETOOLS_API RawSkeletonInfo
    {
        String      m_name;
    };
}