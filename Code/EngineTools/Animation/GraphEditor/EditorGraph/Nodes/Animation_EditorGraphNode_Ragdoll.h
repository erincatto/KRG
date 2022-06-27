#pragma once
#include "EngineTools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class PoweredRagdollEditorNode final : public DataSlotEditorNode
    {
        KRG_REGISTER_TYPE( PoweredRagdollEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Powered Ragdoll"; }
        virtual char const* GetCategory() const override { return "Physics"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual int16_t Compile( GraphCompilationContext& context ) const override;

        virtual char const* const GetDefaultSlotName() const override { return "Ragdoll"; }
        virtual ResourceTypeID GetSlotResourceTypeID() const override;

    private:

        KRG_EXPOSE StringID                m_profileID;
        KRG_EXPOSE float                   m_physicsBlendWeight = 1.0f;
        KRG_EXPOSE bool                    m_isGravityEnabled = false;
    };
}