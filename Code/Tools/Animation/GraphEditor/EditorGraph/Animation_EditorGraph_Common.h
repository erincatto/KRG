#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::VisualGraph { struct DrawContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;

    // Graph Types
    //-------------------------------------------------------------------------

    enum class GraphType
    {
        KRG_REGISTER_ENUM

        BlendTree,
        ValueTree,
        TransitionTree,
    };

    // Debug
    //-------------------------------------------------------------------------

    struct DebugContext
    {
        inline int16_t GetRuntimeGraphNodeIndex( UUID const& nodeID ) const
        {
            auto const foundIter = m_nodeIDtoIndexMap.find( nodeID );
            if ( foundIter != m_nodeIDtoIndexMap.end() )
            {
                return foundIter->second;
            }
            return InvalidIndex;
        }

        bool IsNodeActive( int16_t nodeIdx ) const;
        
        #if KRG_DEVELOPMENT_TOOLS
        PoseNodeDebugInfo GetPoseNodeDebugInfo( int16_t runtimeNodeIdx ) const;
        #endif

        template<typename T>
        inline T GetRuntimeNodeValue( int16_t runtimeNodeIdx ) const
        {
            return m_pGraphComponent->GetRuntimeNodeValue<T>( runtimeNodeIdx );
        }

    public:

        AnimationGraphComponent*             m_pGraphComponent = nullptr;
        THashMap<UUID, int16_t>       m_nodeIDtoIndexMap;
    };

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void DrawPoseNodeDebugInfo( VisualGraph::DrawContext const& ctx, float width, PoseNodeDebugInfo const& debugInfo );
    void DrawEmptyPoseNodeDebugInfo( VisualGraph::DrawContext const& ctx, float width );
    #endif
}