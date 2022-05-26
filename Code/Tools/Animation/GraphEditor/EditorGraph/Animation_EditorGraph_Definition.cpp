#include "Animation_EditorGraph_Definition.h"
#include "Animation_EditorGraph_Compilation.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using namespace KRG::Animation::GraphNodes;

    //-------------------------------------------------------------------------

    EditorGraphDefinition::EditorGraphDefinition()
    {
        ResetToDefaultState();
    }

    EditorGraphDefinition::~EditorGraphDefinition()
    {
        ResetInternalState();
    }

    void EditorGraphDefinition::ResetInternalState()
    {
        m_variationHierarchy.Reset();

        if ( m_pRootGraph != nullptr )
        {
            m_pRootGraph->Shutdown();
            KRG::Delete( m_pRootGraph );
        }
    }

    void EditorGraphDefinition::ResetToDefaultState()
    {
        ResetInternalState();
        m_pRootGraph = KRG::New<FlowGraph>( GraphType::BlendTree );
        m_pRootGraph->CreateNode<ResultEditorNode>( GraphValueType::Pose );
    }

    bool EditorGraphDefinition::LoadFromJson( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphDescriptorObjectValue )
    {
        KRG_ASSERT( graphDescriptorObjectValue.IsObject() );

        ResetInternalState();

        // Find relevant json values
        //-------------------------------------------------------------------------

        RapidJsonValue const* pGraphObjectValue = nullptr;
        RapidJsonValue const* pVariationsObjectValue = nullptr;
        RapidJsonValue const* pRootGraphObjectValue = nullptr;

        auto graphDefinitionValueIter = graphDescriptorObjectValue.FindMember( "GraphDefinition" );
        if ( graphDefinitionValueIter != graphDescriptorObjectValue.MemberEnd() && graphDefinitionValueIter->value.IsObject() )
        {
            pGraphObjectValue = &graphDefinitionValueIter->value;
        }

        if ( pGraphObjectValue != nullptr )
        {
            auto rootGraphValueIter = pGraphObjectValue->FindMember( "RootGraph" );
            if ( rootGraphValueIter != pGraphObjectValue->MemberEnd() )
            {
                pRootGraphObjectValue = &rootGraphValueIter->value;
            }
        }

        if ( pGraphObjectValue != nullptr )
        {
            auto variationsValueIter = pGraphObjectValue->FindMember( "Variations" );
            if ( variationsValueIter != pGraphObjectValue->MemberEnd() && variationsValueIter->value.IsArray() )
            {
                pVariationsObjectValue = &variationsValueIter->value;
            }
        }

        // Deserialize graph
        //-------------------------------------------------------------------------

        bool serializationSuccessful = false;

        if ( pRootGraphObjectValue != nullptr && pVariationsObjectValue != nullptr )
        {
            m_pRootGraph = Cast<FlowGraph>( VisualGraph::BaseGraph::CreateGraphFromSerializedData( typeRegistry, *pRootGraphObjectValue, nullptr ) );
            serializationSuccessful = m_variationHierarchy.Serialize( typeRegistry, *pVariationsObjectValue );
        }

        // If serialization failed, reset the graph state to a valid one
        if ( !serializationSuccessful )
        {
            ResetToDefaultState();
        }

        return serializationSuccessful;
    }

    void EditorGraphDefinition::SaveToJson( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        writer.StartObject();
        writer.Key( "TypeID" );
        writer.String( GraphResourceDescriptor::GetStaticTypeID().c_str() );

        writer.Key( "GraphDefinition" );
        writer.StartObject();
        {
            writer.Key( "RootGraph" );
            m_pRootGraph->Serialize( typeRegistry, writer );

            writer.Key( "Variations" );
            m_variationHierarchy.Serialize( typeRegistry, writer );

            writer.EndObject();
        }
        writer.EndObject();
    }
}