#pragma once
#include "EngineTools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class AnimationClipEditorNode final : public DataSlotEditorNode
    {
        KRG_REGISTER_TYPE( AnimationClipEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Animation Clip"; }
        virtual char const* GetCategory() const override { return "Animation"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual int16_t Compile( GraphCompilationContext& context ) const override;

        virtual char const* const GetDefaultSlotName() const override { return "Animation"; }
        virtual ResourceTypeID GetSlotResourceTypeID() const override { return AnimationClip::GetStaticResourceTypeID(); }

    private:

        KRG_EXPOSE bool         m_sampleRootMotion = true;
        KRG_EXPOSE bool         m_allowLooping = false;
    };

    //-------------------------------------------------------------------------

    // This is a base class for all nodes that directly reference an animation clip
    class AnimationClipReferenceEditorNode : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( AnimationClipEditorNode );
    };
}