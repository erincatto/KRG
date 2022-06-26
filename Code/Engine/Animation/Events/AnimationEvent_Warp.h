#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_API WarpEvent final : public Event
    {
        KRG_REGISTER_TYPE( WarpEvent );

    public:

        // The type of warping we are allowed to perform for this event
        enum class Type : uint8_t
        {
            KRG_REGISTER_ENUM

            Full = 0, // Allows both rotating and stretching/compressing the original motion
            RotationOnly, // Only allows for rotation adjustment of the original motion
        };

    public:

        inline Type GetWarpAdjustmentType() const { return m_type; }

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventName() const override { return "Warp"; }
        virtual InlineString GetDisplayText() const override;
        virtual InlineString GetDebugText() const override { return GetDisplayText(); }
        virtual EventType GetEventType() const override { return EventType::Duration; }
        #endif

    private:

        KRG_EXPOSE Type         m_type = Type::Full;
        KRG_EXPOSE bool         m_allowWarpInZ = true;
    };
} 