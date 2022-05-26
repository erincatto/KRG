#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API WarpEvent final : public Event
    {
        KRG_REGISTER_TYPE( WarpEvent );

    public:

        // The type of warping we are allowed to perform for this event
        enum class Type : uint8
        {
            KRG_REGISTER_ENUM

            RotationAndTranslation = 0,
            Rotation,
            Translation,
        };

    public:

        inline Type GetAllowedWarping() const { return m_type; }
        inline bool IsRotationAllowed() const { return m_type != Type::Translation; }
        inline bool IsTranslationAllowed() const { return m_type != Type::Rotation; }

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventName() const override { return "Warp"; }
        virtual InlineString GetDisplayText() const override;
        virtual InlineString GetDebugText() const override { return GetDisplayText(); }
        virtual EventType GetEventType() const override { return EventType::Duration; }
        #endif

    private:

        KRG_EXPOSE Type         m_type = Type::RotationAndTranslation;
    };
}