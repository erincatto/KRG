#pragma once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Core/TimelineEditor/TimelineEditor.h"
#include "System/FileSystem/FileSystemPath.h"


//-------------------------------------------------------------------------

namespace KRG { class UpdateContext; }
namespace KRG::TypeSystem { class TypeRegistry; }
namespace KRG::Animation { class AnimationClipPlayerComponent; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINETOOLS_API EventEditor final : public Timeline::TimelineEditor
    {
    public:

        EventEditor( TypeSystem::TypeRegistry const& typeRegistry );

        void Reset();
        void SetAnimationLengthAndFPS( uint32_t numFrames, float FPS );
        void UpdateAndDraw( UpdateContext const& context, AnimationClipPlayerComponent* pPreviewAnimationComponent );

        virtual bool Serialize( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonValue const& objectValue ) override;

    private:

        virtual void DrawAddTracksMenu() override;

        void UpdateTimelineTrackFPS();

    private:

        FileSystem::Path const                      m_descriptorPath;
        float                                       m_FPS = 0.0f;
        TVector<TypeSystem::TypeInfo const*>        m_eventTypes;
    };
}