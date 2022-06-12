#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    enum class CachedValueMode
    {
        KRG_REGISTER_ENUM

        OnEntry = 0,
        OnExit
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API CachedBoolNode final : public BoolValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public BoolValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( BoolValueNode::Settings, m_inputValueNodeIdx, m_mode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t      m_inputValueNodeIdx = InvalidIndex;
            CachedValueMode     m_mode;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        BoolValueNode*              m_pInputValueNode = nullptr;
        bool                        m_value;
        bool                        m_hasCachedValue = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API CachedIDNode final : public IDValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IDValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( IDValueNode::Settings, m_inputValueNodeIdx, m_mode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t      m_inputValueNodeIdx = InvalidIndex;
            CachedValueMode     m_mode;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IDValueNode*                m_pInputValueNode = nullptr;
        StringID                    m_value;
        bool                        m_hasCachedValue = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API CachedIntNode final : public IntValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public IntValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( IntValueNode::Settings, m_inputValueNodeIdx, m_mode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t      m_inputValueNodeIdx = InvalidIndex;
            CachedValueMode     m_mode;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        IntValueNode*               m_pInputValueNode = nullptr;
        int32_t                       m_value;
        bool                        m_hasCachedValue = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API CachedFloatNode final : public FloatValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public FloatValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( FloatValueNode::Settings, m_inputValueNodeIdx, m_mode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t      m_inputValueNodeIdx = InvalidIndex;
            CachedValueMode     m_mode;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        FloatValueNode*             m_pInputValueNode = nullptr;
        float                       m_previousValue;
        float                       m_currentValue;
        bool                        m_hasCachedValue = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API CachedVectorNode final : public VectorValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public VectorValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( VectorValueNode::Settings, m_inputValueNodeIdx, m_mode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t      m_inputValueNodeIdx = InvalidIndex;
            CachedValueMode     m_mode;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        VectorValueNode*            m_pInputValueNode = nullptr;
        Vector                      m_value;
        bool                        m_hasCachedValue = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API CachedTargetNode final : public TargetValueNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public TargetValueNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( TargetValueNode::Settings, m_inputValueNodeIdx, m_mode );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            int16_t      m_inputValueNodeIdx = InvalidIndex;
            CachedValueMode     m_mode;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        TargetValueNode*            m_pInputValueNode = nullptr;
        Target                      m_value;
        bool                        m_hasCachedValue = false;
    };
}