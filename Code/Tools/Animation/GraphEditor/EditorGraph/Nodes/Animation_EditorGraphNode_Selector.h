#pragma once

#include "Animation_EditorGraphNode_AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class SelectorConditionEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( SelectorConditionEditorNode );

        friend class SelectorEditorNode;
        friend class AnimationClipSelectorEditorNode;

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Conditions"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
    };

    //-------------------------------------------------------------------------

    class SelectorEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( SelectorEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "Selector"; }
        virtual char const* GetCategory() const override { return "Selectors"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual TInlineString<100> GetNewDynamicInputPinName() const override;
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) GraphValueType::Pose; }
        virtual void OnDynamicPinCreation( UUID pinID ) override;
        virtual void OnDynamicPinDestruction( UUID pinID ) override;
    };

    //-------------------------------------------------------------------------

    class AnimationClipSelectorEditorNode final : public AnimationClipReferenceEditorNode
    {
        KRG_REGISTER_TYPE( AnimationClipSelectorEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "Animation Clip Selector"; }
        virtual char const* GetCategory() const override { return "Selectors"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

        virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const override;
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual TInlineString<100> GetNewDynamicInputPinName() const override;
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) GraphValueType::Pose; }
        virtual void OnDynamicPinCreation( UUID pinID ) override;
        virtual void OnDynamicPinDestruction( UUID pinID ) override;
    };
}