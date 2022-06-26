#pragma once
#include "Engine/Animation/AnimationEvent.h"
#include "System/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_API IDEvent final : public Event
    {
        KRG_REGISTER_TYPE( IDEvent );

    public:

        inline StringID const& GetID() const { return m_ID; }
        virtual StringID GetSyncEventID() const override { return m_ID; }

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetEventName() const override { return "ID"; }
        virtual bool AllowMultipleTracks() const override { return true; }
        virtual InlineString GetDisplayText() const override { return m_ID.IsValid() ? m_ID.c_str() : "Invalid ID"; }
        virtual InlineString GetDebugText() const override { return GetDisplayText(); }
        virtual EventType GetEventType() const override { return EventType::Both; }
        #endif

    private:

        KRG_EXPOSE StringID         m_ID;
    };
}