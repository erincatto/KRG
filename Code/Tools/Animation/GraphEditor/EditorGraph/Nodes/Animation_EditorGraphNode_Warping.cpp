#include "Animation_EditorGraphNode_Warping.h"
#include "Animation_EditorGraphNode_AnimationClip.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Warping.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void OrientationWarpEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Pose, true );
        CreateInputPin( "Input", GraphValueType::Pose );
        CreateInputPin( "Angle Offset", GraphValueType::Float );
    }

    GraphNodeIndex OrientationWarpEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        OrientationWarpNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<OrientationWarpNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_clipReferenceNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<EditorGraphNode>( 1 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_angleOffsetValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }
        }
        return pSettings->m_nodeIdx;
    }

    bool OrientationWarpEditorNode::IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const
    {
        int32 const pinIdx = GetInputPinIndex( inputPinID );
        if ( pinIdx == 0 )
        {
            return IsOfType<AnimationClipEditorNode>( pOutputPinNode ) || IsOfType<AnimationClipReferenceEditorNode>( pOutputPinNode );
        }

        return EditorGraphNode::IsValidConnection( inputPinID, pOutputPinNode, outputPinID );
    }

    //-------------------------------------------------------------------------

    void TargetWarpEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Pose, true );
        CreateInputPin( "Input", GraphValueType::Pose );
        CreateInputPin( "Target", GraphValueType::Target );
    }

    GraphNodeIndex TargetWarpEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        TargetWarpNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TargetWarpNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_clipReferenceNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<EditorGraphNode>( 1 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_targetValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }
        }
        return pSettings->m_nodeIdx;
    }

    bool TargetWarpEditorNode::IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const
    {
        int32 const pinIdx = GetInputPinIndex( inputPinID );
        if ( pinIdx == 0 )
        {
            return IsOfType<AnimationClipEditorNode>( pOutputPinNode ) || IsOfType<AnimationClipReferenceEditorNode>( pOutputPinNode );
        }

        return EditorGraphNode::IsValidConnection( inputPinID, pOutputPinNode, outputPinID );
    }
}