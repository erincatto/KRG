#include "AnimationEvent_Warp.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    #if KRG_DEVELOPMENT_TOOLS
    InlineString WarpEvent::GetDisplayText() const
    {
        if ( m_type == Type::RotationOnly )
        {
            return "Rotation Only";
        }

        return "Full";
    }
    #endif
}