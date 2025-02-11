#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_API FootEvent final : public Event
    {
        KRG_REGISTER_TYPE( FootEvent );

    public:

        // The actual foot phase
        enum class Phase : uint8_t
        {
            KRG_REGISTER_ENUM

            LeftFootDown = 0,
            RightFootPassing = 1,
            RightFootDown = 2,
            LeftFootPassing = 3,
        };

        // Used wherever we need to specify a phase (or phase group)
        enum class PhaseCondition : uint8_t
        {
            KRG_REGISTER_ENUM

            LeftFootDown = 0,
            LeftFootPassing = 1,
            LeftPhase = 4, // The whole phase for the right foot down (right foot down + left foot passing)

            RightFootDown = 2,
            RightFootPassing = 3,
            RightPhase = 5, // The whole phase for the right foot down (right foot down + left foot passing)
        };

    public:

        inline Phase GetFootPhase() const { return m_phase; }
        virtual StringID GetSyncEventID() const override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventName() const override { return "Foot"; }
        virtual InlineString GetDisplayText() const override;
        virtual InlineString GetDebugText() const override { return GetDisplayText(); }
        virtual EventType GetEventType() const override { return EventType::Duration; }
        #endif

    private:

        KRG_EXPOSE Phase         m_phase = Phase::LeftFootDown;
    };
}