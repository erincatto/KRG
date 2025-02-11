#include "AnimationGraphEditor_Context.h"
#include "EditorGraph/Nodes/Animation_EditorGraphNode_ControlParameters.h"
#include "EngineTools/Core/ToolsContext.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

using namespace KRG::Animation::GraphNodes;

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    GraphEditorContext::GraphEditorContext( ToolsContext const& toolsContext )
        : m_toolsContext( toolsContext )
    {
        KRG_ASSERT( m_toolsContext.IsValid() );

        // Create DB of all node types
        //-------------------------------------------------------------------------

        m_registeredNodeTypes = toolsContext.m_pTypeRegistry->GetAllDerivedTypes( EditorGraphNode::GetStaticTypeID(), false, false, true );

        for ( auto pNodeType : m_registeredNodeTypes )
        {
            auto pDefaultNode = Cast<EditorGraphNode const>( pNodeType->m_pDefaultInstance );
            if ( pDefaultNode->IsUserCreatable() )
            {
                m_categorizedNodeTypes.AddItem( pDefaultNode->GetCategory(), pDefaultNode->GetTypeName(), pNodeType );
            }
        }
    }

    //-------------------------------------------------------------------------

    bool GraphEditorContext::LoadGraph( Serialization::JsonValue const& graphDescriptorObjectValue )
    {
        m_controlParameters.clear();
        m_virtualParameters.clear();

        if ( m_editorGraph.LoadFromJson( *m_toolsContext.m_pTypeRegistry, graphDescriptorObjectValue ) )
        {
            auto pRootGraph = GetRootGraph();
            m_controlParameters = pRootGraph->FindAllNodesOfType<ControlParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Derived );
            m_virtualParameters = pRootGraph->FindAllNodesOfType<VirtualParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Exact );
            return true;
        }

        return false;
    }

    void GraphEditorContext::SaveGraph( Serialization::JsonWriter& writer ) const
    {
        return m_editorGraph.SaveToJson( *m_toolsContext.m_pTypeRegistry, writer );
    }

    //-------------------------------------------------------------------------

    void GraphEditorContext::CreateControlParameter( GraphValueType type )
    {
        String parameterName = "Parameter";
        EnsureUniqueParameterName( parameterName );

        //-------------------------------------------------------------------------

        ControlParameterEditorNode* pParameter = nullptr;

        VisualGraph::ScopedGraphModification gm( GetRootGraph() );

        switch ( type )
        {
            case GraphValueType::Bool:
            pParameter = GetRootGraph()->CreateNode<BoolControlParameterEditorNode>( parameterName );
            break;

            case GraphValueType::ID:
            pParameter = GetRootGraph()->CreateNode<IDControlParameterEditorNode>( parameterName );
            break;

            case GraphValueType::Int:
            pParameter = GetRootGraph()->CreateNode<IntControlParameterEditorNode>( parameterName );
            break;

            case GraphValueType::Float:
            pParameter = GetRootGraph()->CreateNode<FloatControlParameterEditorNode>( parameterName );
            break;

            case GraphValueType::Vector:
            pParameter = GetRootGraph()->CreateNode<VectorControlParameterEditorNode>( parameterName );
            break;

            case GraphValueType::Target:
            pParameter = GetRootGraph()->CreateNode<TargetControlParameterEditorNode>( parameterName );
            break;

            default:
            break;
        }

        KRG_ASSERT( pParameter != nullptr );
        m_controlParameters.emplace_back( pParameter );
    }

    void GraphEditorContext::CreateVirtualParameter( GraphValueType type )
    {
        String parameterName = "Parameter";
        EnsureUniqueParameterName( parameterName );

        VisualGraph::ScopedGraphModification gm( GetRootGraph() );
        auto pParameter = GetRootGraph()->CreateNode<VirtualParameterEditorNode>( parameterName, type );
        m_virtualParameters.emplace_back( pParameter );
    }

    void GraphEditorContext::RenameControlParameter( UUID parameterID, String const& newName, String const& category )
    {
        auto pParameter = FindControlParameter( parameterID );
        KRG_ASSERT( pParameter != nullptr );

        String uniqueName = newName;
        EnsureUniqueParameterName( uniqueName );

        VisualGraph::ScopedGraphModification gm( GetRootGraph() );
        pParameter->Rename( uniqueName, category );
    }

    void GraphEditorContext::RenameVirtualParameter( UUID parameterID, String const& newName, String const& category )
    {
        auto pParameter = FindVirtualParameter( parameterID );
        KRG_ASSERT( pParameter != nullptr );

        String uniqueName = newName;
        EnsureUniqueParameterName( uniqueName );

        VisualGraph::ScopedGraphModification gm( GetRootGraph() );
        pParameter->Rename( uniqueName, category );
    }

    void GraphEditorContext::DestroyControlParameter( UUID parameterID )
    {
        KRG_ASSERT( FindControlParameter( parameterID ) != nullptr );

        VisualGraph::ScopedGraphModification gm( GetRootGraph() );

        // Find and remove all reference nodes
        auto const parameterReferences = FindAllNodesOfType<ParameterReferenceEditorNode>( VisualGraph::SearchMode::Recursive, VisualGraph::SearchTypeMatch::Exact );
        for ( auto const& pFoundParameterNode : parameterReferences )
        {
            if ( pFoundParameterNode->GetReferencedParameterID() == parameterID )
            {
                pFoundParameterNode->Destroy();
            }
        }

        // Delete parameter
        for ( auto iter = m_controlParameters.begin(); iter != m_controlParameters.end(); ++iter )
        {
            auto pParameter = ( *iter );
            if ( pParameter->GetID() == parameterID )
            {
                pParameter->Destroy();
                m_controlParameters.erase( iter );
                break;
            }
        }
    }

    void GraphEditorContext::DestroyVirtualParameter( UUID parameterID )
    {
        KRG_ASSERT( FindVirtualParameter( parameterID ) != nullptr );

        VisualGraph::ScopedGraphModification gm( GetRootGraph() );

        // Find and remove all reference nodes
        auto const parameterReferences = FindAllNodesOfType<ParameterReferenceEditorNode>( VisualGraph::SearchMode::Recursive, VisualGraph::SearchTypeMatch::Exact );
        for ( auto const& pFoundParameterNode : parameterReferences )
        {
            if ( pFoundParameterNode->GetReferencedParameterID() == parameterID )
            {
                pFoundParameterNode->Destroy();
            }
        }

        // Delete parameter
        for ( auto iter = m_virtualParameters.begin(); iter != m_virtualParameters.end(); ++iter )
        {
            auto pParameter = ( *iter );
            if ( pParameter->GetID() == parameterID )
            {
                pParameter->Destroy();
                m_virtualParameters.erase( iter );
                break;
            }
        }
    }

    void GraphEditorContext::EnsureUniqueParameterName( String& parameterName ) const
    {
        String tempString = parameterName;
        bool isNameUnique = false;
        int32_t suffix = 0;

        while ( !isNameUnique )
        {
            isNameUnique = true;

            // Check control parameters
            for ( auto pParameter : m_controlParameters )
            {
                if ( pParameter->GetParameterName() == tempString )
                {
                    isNameUnique = false;
                    break;
                }
            }

            // Check virtual parameters
            if ( isNameUnique )
            {
                for ( auto pParameter : m_virtualParameters )
                {
                    if ( pParameter->GetParameterName() == tempString )
                    {
                        isNameUnique = false;
                        break;
                    }
                }
            }

            if ( !isNameUnique )
            {
                tempString.sprintf( "%s_%d", parameterName.c_str(), suffix );
                suffix++;
            }
        }

        //-------------------------------------------------------------------------

        parameterName = tempString;
    }

    ControlParameterEditorNode* GraphEditorContext::FindControlParameter( UUID parameterID ) const
    {
        for ( auto pParameter : m_controlParameters )
        {
            if ( pParameter->GetID() == parameterID )
            {
                return pParameter;
            }
        }
        return nullptr;
    }

    VirtualParameterEditorNode* GraphEditorContext::FindVirtualParameter( UUID parameterID ) const
    {
        for ( auto pParameter : m_virtualParameters )
        {
            if ( pParameter->GetID() == parameterID )
            {
                return pParameter;
            }
        }
        return nullptr;
    }

    //-------------------------------------------------------------------------

    void GraphEditorContext::CreateVariation( StringID variationID, StringID parentVariationID )
    {
        VisualGraph::ScopedGraphModification gm( GetRootGraph() );
        m_editorGraph.GetVariationHierarchy().CreateVariation( variationID, parentVariationID );
    }

    void GraphEditorContext::RenameVariation( StringID existingVariationID, StringID newVariationID )
    {
        auto pRootGraph = GetRootGraph();
        VisualGraph::ScopedGraphModification gm( pRootGraph );

        // Rename actual variation
        m_editorGraph.GetVariationHierarchy().RenameVariation( existingVariationID, newVariationID );

        // Update all data slot nodes
        auto dataSlotNodes = pRootGraph->FindAllNodesOfType<GraphNodes::DataSlotEditorNode>( VisualGraph::SearchMode::Recursive, VisualGraph::SearchTypeMatch::Derived );
        for ( auto pDataSlotNode : dataSlotNodes )
        {
            pDataSlotNode->RenameOverride( existingVariationID, newVariationID );
        }
    }

    void GraphEditorContext::DestroyVariation( StringID variationID )
    {
        VisualGraph::ScopedGraphModification gm( GetRootGraph() );
        m_editorGraph.GetVariationHierarchy().DestroyVariation( variationID );
    }

    void GraphEditorContext::NavigateTo( UUID const& nodeID )
    {
        auto const pFoundNode = m_editorGraph.GetRootGraph()->FindNode( nodeID, true );
        NavigateTo( pFoundNode );
    }

    void GraphEditorContext::NavigateTo( VisualGraph::BaseNode* pNode )
    {
        if ( pNode != nullptr )
        {
            m_onNavigateToNode.Execute( pNode );
        }
    }

    void GraphEditorContext::NavigateTo( VisualGraph::BaseGraph* pGraph )
    {
        if ( pGraph != nullptr )
        {
            m_onNavigateToGraph.Execute( pGraph );
        }
    }
}