#pragma once

#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class OrientationWarpEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( OrientationWarpEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;
        virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const override;
        virtual char const* GetTypeName() const override { return "Orientation Warp"; }
        virtual char const* GetCategory() const override { return "Motion Warping"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class TargetWarpEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( TargetWarpEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;
        virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const override;
        virtual char const* GetTypeName() const override { return "Target Warp"; }
        virtual char const* GetCategory() const override { return "Motion Warping"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };
}