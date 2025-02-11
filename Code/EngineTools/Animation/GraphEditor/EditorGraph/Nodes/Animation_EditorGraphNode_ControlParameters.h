#pragma once
#include "EngineTools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class BoolControlParameterEditorNode : public ControlParameterEditorNode
    {
        KRG_REGISTER_TYPE( BoolControlParameterEditorNode );

    public:

        using ControlParameterEditorNode::ControlParameterEditorNode;

        virtual void Initialize( VisualGraph::BaseGraph* pParentGraph ) override;

        inline bool GetPreviewStartValue() const { return m_previewStartValue; }

    private:

        KRG_EXPOSE bool m_previewStartValue = false;
    };

    //-------------------------------------------------------------------------

    class FloatControlParameterEditorNode : public ControlParameterEditorNode
    {
        KRG_REGISTER_TYPE( FloatControlParameterEditorNode );

    public:

        using ControlParameterEditorNode::ControlParameterEditorNode;

        virtual void Initialize( VisualGraph::BaseGraph* pParentGraph ) override;

        inline float GetPreviewStartValue() const { return m_previewStartValue; }
        inline float GetPreviewRangeMin() const { return m_previewMin; }
        inline float GetPreviewRangeMax() const { return m_previewMax; }

    private:

        KRG_EXPOSE float m_previewStartValue = 0;
        KRG_EXPOSE float m_previewMin = 0;
        KRG_EXPOSE float m_previewMax = 1;
    };

    //-------------------------------------------------------------------------

    class IntControlParameterEditorNode : public ControlParameterEditorNode
    {
        KRG_REGISTER_TYPE( IntControlParameterEditorNode );

    public:

        using ControlParameterEditorNode::ControlParameterEditorNode;

        virtual void Initialize( VisualGraph::BaseGraph* pParentGraph ) override;

        inline int32_t GetPreviewStartValue() const { return m_previewStartValue; }

    private:

        KRG_EXPOSE int32_t m_previewStartValue = 0;
    };

    //-------------------------------------------------------------------------

    class IDControlParameterEditorNode : public ControlParameterEditorNode
    {
        KRG_REGISTER_TYPE( IDControlParameterEditorNode );

    public:

        using ControlParameterEditorNode::ControlParameterEditorNode;

        virtual void Initialize( VisualGraph::BaseGraph* pParentGraph ) override;

        inline StringID GetPreviewStartValue() const { return m_previewStartValue; }

    private:

        KRG_EXPOSE StringID m_previewStartValue;
    };

    //-------------------------------------------------------------------------

    class VectorControlParameterEditorNode : public ControlParameterEditorNode
    {
        KRG_REGISTER_TYPE( VectorControlParameterEditorNode );

    public:

        using ControlParameterEditorNode::ControlParameterEditorNode;

        virtual void Initialize( VisualGraph::BaseGraph* pParentGraph ) override;

        inline Vector GetPreviewStartValue() const { return m_previewStartValue; }

    private:

        KRG_EXPOSE Vector m_previewStartValue = Vector::Zero;
    };

    //-------------------------------------------------------------------------

    class TargetControlParameterEditorNode : public ControlParameterEditorNode
    {
        KRG_REGISTER_TYPE( TargetControlParameterEditorNode );

    public:

        using ControlParameterEditorNode::ControlParameterEditorNode;

        virtual void Initialize( VisualGraph::BaseGraph* pParentGraph ) override;

        inline Target GetPreviewStartValue() const
        {
            Target target;

            if ( m_isSet )
            {
                if ( m_isBoneID )
                {
                    target = Target( m_previewStartBoneID );
                }
                else
                {
                    target = Target( m_previewStartTransform );
                }
            }

            return target;
        }

        inline bool IsBoneTarget() const { return m_isBoneID; }
        inline Transform const& GetPreviewTargetTransform() const { return m_previewStartTransform; }
        void SetPreviewTargetTransform( Transform const& transform );

    private:

        KRG_EXPOSE bool         m_isSet = false;
        KRG_EXPOSE bool         m_isBoneID = false;
        KRG_EXPOSE Transform    m_previewStartTransform = Transform::Identity;
        KRG_EXPOSE StringID     m_previewStartBoneID;
    };
}