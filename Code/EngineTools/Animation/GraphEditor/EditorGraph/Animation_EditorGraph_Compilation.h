#pragma once
#include "Animation_EditorGraph_FlowGraph.h"
#include "System/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    enum NodeCompilationState
    {
        NeedCompilation,
        AlreadyCompiled,
    };

    //-------------------------------------------------------------------------

    struct NodeCompilationLogEntry
    {
        NodeCompilationLogEntry( Log::Severity severity, UUID const& nodeID, String const& message )
            : m_message( message )
            , m_nodeID( nodeID )
            , m_severity( severity )
        {}

        String              m_message;
        UUID                m_nodeID;
        Log::Severity       m_severity;
    };

    //-------------------------------------------------------------------------

    class GraphCompilationContext
    {
        friend class GraphDefinitionCompiler;

    public:

        GraphCompilationContext();
        ~GraphCompilationContext();

        void Reset();

        // Logging
        //-------------------------------------------------------------------------

        void LogMessage( VisualGraph::BaseNode const* pNode, String const& message ) { m_log.emplace_back( NodeCompilationLogEntry( Log::Severity::Message, pNode->GetID(), message ) ); }
        void LogWarning( VisualGraph::BaseNode const* pNode, String const& message ) { m_log.emplace_back( NodeCompilationLogEntry( Log::Severity::Warning, pNode->GetID(), message ) ); }
        void LogError( VisualGraph::BaseNode const* pNode, String const& message ) { m_log.emplace_back( NodeCompilationLogEntry( Log::Severity::Error, pNode->GetID(), message ) ); }

        // General Compilation
        //-------------------------------------------------------------------------

        inline THashMap<UUID, int16_t> GetIDToIndexMap() const { return m_nodeIDToIndexMap; }

        // Try to get the runtime settings for a node in the graph, will return whether this node was already compiled or still needs compilation
        template<typename T>
        NodeCompilationState GetSettings( VisualGraph::BaseNode const* pNode, typename T::Settings*& pOutSettings )
        {
            auto foundIter = m_nodeIDToIndexMap.find( pNode->GetID() );
            if ( foundIter != m_nodeIDToIndexMap.end() )
            {
                pOutSettings = (T::Settings*) m_nodeSettings[foundIter->second];
                return NodeCompilationState::AlreadyCompiled;
            }

            //-------------------------------------------------------------------------

            KRG_ASSERT( m_nodeSettings.size() < 0xFFFF );
            pOutSettings = KRG::New<T::Settings>();
            m_nodeSettings.emplace_back( pOutSettings );
            m_compiledNodePaths.emplace_back( pNode->GetPathFromRoot() );
            pOutSettings->m_nodeIdx = int16_t( m_nodeSettings.size() - 1 );

            // Add to map
            m_nodeIDToIndexMap.insert( TPair<UUID, int16_t>( pNode->GetID(), pOutSettings->m_nodeIdx ) );

            // Add to persistent nodes list
            auto pFlowNode = TryCast<GraphNodes::EditorGraphNode>( pNode );
            if ( pFlowNode != nullptr && pFlowNode->IsPersistentNode() )
            {
                m_persistentNodeIndices.emplace_back( pOutSettings->m_nodeIdx );
            }

            // Update instance requirements
            m_graphInstanceRequiredAlignment = Math::Max( m_graphInstanceRequiredAlignment, ( uint32_t ) alignof( T ) );
            size_t const requiredPadding = Memory::CalculatePaddingForAlignment( m_currentNodeMemoryOffset, alignof( T ) );
            m_currentNodeMemoryOffset += uint32_t( sizeof( T ) + requiredPadding );
            m_nodeMemoryOffsets.emplace_back( m_currentNodeMemoryOffset );

            return NodeCompilationState::NeedCompilation;
        }

        // This will return an index that can be used to look up the data resource at runtime
        inline DataSetSlotIndex RegisterSlotNode( UUID const& nodeID )
        {
            KRG_ASSERT( !VectorContains( m_registeredDataSlots, nodeID ) );

            DataSetSlotIndex slotIdx = (DataSetSlotIndex) m_registeredDataSlots.size();
            m_registeredDataSlots.emplace_back( nodeID );
            return slotIdx;
        }

        // State Machine Compilation
        //-------------------------------------------------------------------------

        // Start compilation of a transition conduit
        inline void BeginConduitCompilation( int16_t sourceStateNodeIdx )
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx == InvalidIndex );
            KRG_ASSERT( sourceStateNodeIdx != InvalidIndex );
            m_conduitSourceStateCompiledNodeIdx = sourceStateNodeIdx;
        }

        // End compilation of a transition conduit
        inline void EndConduitCompilation()
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            m_conduitSourceStateCompiledNodeIdx = InvalidIndex;
        }

        // Some nodes optionally need the conduit index so we need to have a way to know what mode we are in
        inline int16_t IsCompilingConduit() const
        {
            return m_conduitSourceStateCompiledNodeIdx != InvalidIndex;
        }

        inline int16_t GetConduitSourceStateIndex() const
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            return m_conduitSourceStateCompiledNodeIdx;
        }

        // Start compilation of a transition conduit
        inline void BeginTransitionConditionsCompilation( Seconds transitionDuration, int16_t transitionDurationOverrideIdx )
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            m_transitionDuration = transitionDuration;
            m_transitionDurationOverrideIdx = transitionDurationOverrideIdx;
        }

        // End compilation of a transition conduit
        inline void EndTransitionConditionsCompilation()
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            m_transitionDuration = 0;
            m_transitionDurationOverrideIdx = InvalidIndex;
        }

        inline int16_t GetCompiledTransitionDurationOverrideIdx() const
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            return m_transitionDurationOverrideIdx;
        }

        inline Seconds GetCompiledTransitionDuration() const
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            return m_transitionDuration;
        }

    private:

        TVector<NodeCompilationLogEntry>        m_log;
        THashMap<UUID, int16_t>          m_nodeIDToIndexMap;
        TVector<int16_t>                 m_persistentNodeIndices;
        TVector<String>                         m_compiledNodePaths;
        TVector<GraphNode::Settings*>           m_nodeSettings;
        TVector<uint32_t>                         m_nodeMemoryOffsets;
        uint32_t                                  m_currentNodeMemoryOffset = 0;
        uint32_t                                  m_graphInstanceRequiredAlignment = alignof( bool );

        TVector<UUID>                           m_registeredDataSlots;
        int16_t                          m_conduitSourceStateCompiledNodeIdx = InvalidIndex;
        Seconds                                 m_transitionDuration = 0;
        int16_t                          m_transitionDurationOverrideIdx = InvalidIndex;
    };
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class EditorGraphDefinition;
    class GraphDefinition;

    //-------------------------------------------------------------------------

    class GraphDefinitionCompiler
    {

    public:

        bool CompileGraph( EditorGraphDefinition const& editorGraph );

        inline GraphDefinition const* GetCompiledGraph() const { return &m_runtimeGraph; }
        inline TVector<NodeCompilationLogEntry> const& GetLog() const { return m_context.m_log; }
        inline TVector<UUID> const& GetRegisteredDataSlots() const { return m_context.m_registeredDataSlots; }
        inline THashMap<UUID, int16_t> const& GetIDToIndexMap() const { return m_context.m_nodeIDToIndexMap; }

    private:

        GraphDefinition             m_runtimeGraph;
        GraphCompilationContext     m_context;
    };
}