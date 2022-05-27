#include "Animation_RuntimeGraph_Common.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    Color GetColorForValueType( GraphValueType type )
    {
        static const Color colors[9] =
        {
            Colors::GhostWhite,
            Colors::PaleGreen,
            Colors::Orange,
            Colors::Violet,
            Colors::PaleVioletRed,
            Colors::DeepSkyBlue,
            Colors::Cyan,
            Colors::PeachPuff,
            Colors::GreenYellow,
        };

        return colors[(uint8_t) type];
    }

    char const* GetNameForValueType( GraphValueType type )
    {
        switch ( type )
        {
            case GraphValueType::Bool:
            {
                return "Bool";
            }
            break;

            case GraphValueType::ID:
            {
                return "ID";
            }
            break;

            case GraphValueType::Int:
            {
                return "Int";
            }
            break;

            case GraphValueType::Float:
            {
                return "Float";
            }
            break;

            case GraphValueType::Vector:
            {
                return "Vector";
            }
            break;

            case GraphValueType::Target:
            {
                return "Target";
            }
            break;

            case GraphValueType::BoneMask:
            {
                return "Bone Mask";
            }
            break;

            case GraphValueType::Pose:
            {
                return "Pose";
            }
            break;
        }

        return "Unknown";
    }
}