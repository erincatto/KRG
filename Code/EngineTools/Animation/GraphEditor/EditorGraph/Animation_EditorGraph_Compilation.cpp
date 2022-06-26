#include "Animation_EditorGraph_Compilation.h"
#include "Animation_EditorGraph_Definition.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"
#include "Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using namespace GraphNodes;

    //-------------------------------------------------------------------------

    GraphCompilationContext::GraphCompilationContext()
    {
        m_nodeMemoryOffsets.emplace_back( 0 );
    }


    GraphCompilationContext::~GraphCompilationContext()
    {
        Reset();
    }

    void GraphCompilationContext::Reset()
    {
        for ( auto pSettings : m_nodeSettings )
        {
            KRG::Delete( pSettings );
        }

        //-------------------------------------------------------------------------

        m_log.clear();
        m_nodeIDToIndexMap.clear();
        m_persistentNodeIndices.clear();
        m_compiledNodePaths.clear();
        m_currentNodeMemoryOffset = 0;
        m_graphInstanceRequiredAlignment = alignof( bool );

        m_registeredDataSlots.clear();
        m_conduitSourceStateCompiledNodeIdx = InvalidIndex;
        m_transitionDuration = 0;
        m_transitionDurationOverrideIdx = InvalidIndex;

        m_nodeMemoryOffsets.clear();
        m_nodeMemoryOffsets.emplace_back( 0 );
    }

    //-------------------------------------------------------------------------

    bool GraphDefinitionCompiler::CompileGraph( EditorGraphDefinition const& editorGraph )
    {
        KRG_ASSERT( editorGraph.IsValid() );
        auto pRootGraph = editorGraph.GetRootGraph();

        m_context.Reset();
        m_runtimeGraph = GraphDefinition();

        // Always compile control parameters first
        //-------------------------------------------------------------------------

        auto const controlParameters = pRootGraph->FindAllNodesOfType<ControlParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Derived );
        uint32_t const numControlParameters = (uint32_t) controlParameters.size();
        for ( auto pParameter : controlParameters )
        {
            if ( pParameter->Compile( m_context ) == InvalidIndex )
            {
                return false;
            }
        }

        // Then compile virtual parameters
        //-------------------------------------------------------------------------

        auto const virtualParameters = pRootGraph->FindAllNodesOfType<VirtualParameterEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Exact );
        for ( auto pParameter : virtualParameters )
        {
            if ( pParameter->Compile( m_context ) == InvalidIndex )
            {
                return false;
            }
        }

        // Finally compile the actual graph
        //-------------------------------------------------------------------------

        auto const resultNodes = pRootGraph->FindAllNodesOfType<ResultEditorNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        int16_t const rootNodeIdx = resultNodes[0]->Compile( m_context );
        m_context.m_persistentNodeIndices.emplace_back( rootNodeIdx );

        // The last offset is actual the required memory
        uint32_t const requiredMemory = m_context.m_nodeMemoryOffsets.back();
        m_context.m_nodeMemoryOffsets.pop_back();

        // Fill runtime definition
        //-------------------------------------------------------------------------

        m_runtimeGraph.m_nodeSettings = m_context.m_nodeSettings;
        m_runtimeGraph.m_persistentNodeIndices = m_context.m_persistentNodeIndices;
        m_runtimeGraph.m_instanceNodeStartOffsets = m_context.m_nodeMemoryOffsets;
        m_runtimeGraph.m_instanceRequiredMemory = requiredMemory;
        m_runtimeGraph.m_instanceRequiredAlignment = m_context.m_graphInstanceRequiredAlignment;
        m_runtimeGraph.m_numControlParameters = numControlParameters;
        m_runtimeGraph.m_rootNodeIdx = rootNodeIdx;

        for ( uint32_t i = 0; i < numControlParameters; i++ )
        {
            m_runtimeGraph.m_controlParameterIDs.emplace_back( controlParameters[i]->GetParameterID() );
        }

        #if KRG_DEVELOPMENT_TOOLS
        m_runtimeGraph.m_nodePaths.swap( m_context.m_compiledNodePaths );
        #endif

        //-------------------------------------------------------------------------

        return m_runtimeGraph.m_rootNodeIdx != InvalidIndex;
    }
}