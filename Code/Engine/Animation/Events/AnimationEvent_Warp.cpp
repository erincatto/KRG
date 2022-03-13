#include "AnimationEvent_Warp.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    #if KRG_DEVELOPMENT_TOOLS
    InlineString WarpEvent::GetDisplayText() const
    {
        if ( m_type == Type::Rotation )
        {
            return "Rot";
        }
        else if ( m_type == Type::Translation )
        {
            return "Trans";
        }
        else
        {
            return "Rot & Trans";
        }
    }
    #endif
}