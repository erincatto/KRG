#pragma once

#include "Engine/_Module/API.h"
#include "System/Time/Time.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Math/NumericRange.h"

//-------------------------------------------------------------------------
// Animation Event
//-------------------------------------------------------------------------
// Base class for all animation events

namespace KRG::Animation
{
    class KRG_ENGINE_API Event : public IRegisteredType
    {
        KRG_REGISTER_TYPE( Event );

        friend struct EventManipulator;
        friend class AnimationClipCompiler;

    public:

        enum class EventType
        {
            KRG_REGISTER_ENUM

            Immediate,
            Duration,
            Both
        };

    public:

        Event() = default;
        virtual ~Event() = default;

        inline Seconds GetStartTime() const { return m_startTime; }
        inline Seconds GetDuration() const { return m_duration; }
        inline Seconds GetEndTime() const { KRG_ASSERT( IsDurationEvent() ); return m_startTime + m_duration; }
        inline bool IsImmediateEvent() const { return m_duration == 0; }
        inline bool IsDurationEvent() const { return m_duration > 0; }

        // Get the time range for this event (in seconds)
        KRG_FORCE_INLINE FloatRange GetTimeRange() const { return FloatRange( m_startTime, m_startTime + m_duration ); }

        // Optional: Allow the track to return a specific sync event ID
        virtual StringID GetSyncEventID() const { return StringID(); }

        // Event properties
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        virtual InlineString GetDisplayText() const = 0;
        virtual InlineString GetDebugText() const { return GetStaticTypeID().c_str(); }
        virtual char const* GetEventName() const = 0;
        virtual bool AllowMultipleTracks() const { return false; }
        virtual int32_t GetMaxEventsAllowedPerTrack() const { return -1; }
        virtual EventType GetEventType() const { return EventType::Duration; }
        #endif

    protected:

        KRG_REGISTER Seconds         m_startTime = 0.0f;
        KRG_REGISTER Seconds         m_duration = 0.0f;
    };
}