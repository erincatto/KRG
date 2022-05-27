#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/TimelineEditor/TimelineEditor.h"
#include "System/Core/FileSystem/FileSystemPath.h"


//-------------------------------------------------------------------------

namespace KRG { class UpdateContext; }
namespace KRG::TypeSystem { class TypeRegistry; }
namespace KRG::Animation { class AnimationClipPlayerComponent; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_TOOLS_ANIMATION_API EventEditor final : public Timeline::TimelineEditor
    {
    public:

        EventEditor( TypeSystem::TypeRegistry const& typeRegistry );

        void Reset();
        void SetAnimationLengthAndFPS( uint32_t numFrames, float FPS );
        void UpdateAndDraw( UpdateContext const& context, AnimationClipPlayerComponent* pPreviewAnimationComponent );

        virtual bool Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& objectValue ) override;

    private:

        virtual void DrawAddTracksMenu() override;

        void UpdateTimelineTrackFPS();

    private:

        FileSystem::Path const                      m_descriptorPath;
        float                                       m_FPS = 0.0f;
        TVector<TypeSystem::TypeInfo const*>        m_eventTypes;
    };
}