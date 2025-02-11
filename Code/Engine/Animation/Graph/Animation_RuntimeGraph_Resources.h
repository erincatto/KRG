#pragma once
#include "Animation_RuntimeGraph_Node.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Animation/AnimationSkeleton.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using DataSetSlotIndex = int16_t;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API GraphDataSet : public Resource::IResource
    {
        KRG_REGISTER_VIRTUAL_RESOURCE( 'agds', "Animation Graph DataSet" );
        KRG_SERIALIZE( m_variationID, m_pSkeleton, m_resources );
        friend class AnimationGraphCompiler;
        friend class GraphLoader;
        friend class GraphInstance;

    public:

        virtual bool IsValid() const override { return m_variationID.IsValid() && m_pSkeleton.IsLoaded(); }

        inline Skeleton const* GetSkeleton() const { return m_pSkeleton.GetPtr(); }

        template<typename T>
        inline T const* GetResource( DataSetSlotIndex const& ID ) const
        {
            KRG_ASSERT( ID >= 0 && ID < m_resources.size() );
            if ( m_resources[ID].IsValid() )
            {
                return TResourcePtr<T>( m_resources[ID] ).GetPtr();
            }

            return nullptr;
        }

    private:

        StringID                                    m_variationID;
        TResourcePtr<Skeleton>                      m_pSkeleton = nullptr;
        TVector<Resource::ResourcePtr>              m_resources;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API GraphDefinition : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'ag', "Animation Graph" );
        KRG_SERIALIZE( m_persistentNodeIndices, m_instanceNodeStartOffsets, m_instanceRequiredMemory, m_instanceRequiredAlignment, m_numControlParameters, m_rootNodeIdx, m_controlParameterIDs );

        friend class GraphDefinitionCompiler;
        friend class AnimationGraphCompiler;
        friend class GraphLoader;
        friend class GraphInstance;

    public:

        virtual bool IsValid() const override { return m_rootNodeIdx != InvalidIndex; }

        #if KRG_DEVELOPMENT_TOOLS
        String const& GetNodePath( int16_t nodeIdx ) const{ return m_nodePaths[nodeIdx]; }
        #endif

    protected:

        TVector<int16_t>                     m_persistentNodeIndices;
        TVector<uint32_t>                           m_instanceNodeStartOffsets;
        uint32_t                                    m_instanceRequiredMemory = 0;
        uint32_t                                    m_instanceRequiredAlignment = 0;
        int32_t                                     m_numControlParameters = 0;
        int16_t                              m_rootNodeIdx = InvalidIndex;
        TVector<StringID>                           m_controlParameterIDs;

        #if KRG_DEVELOPMENT_TOOLS
        TVector<String>                             m_nodePaths;
        #endif

        // Node settings are created/destroyed by the animation graph loader
        TVector<GraphNode::Settings*>               m_nodeSettings;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API GraphVariation : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'agv', "Animation Graph Variation" );
        KRG_SERIALIZE( m_pGraphDefinition, m_pDataSet );

        friend class AnimationGraphCompiler;
        friend class GraphLoader;
        friend class GraphInstance;

    public:

        static StringID const DefaultVariationID;

    public:

        virtual bool IsValid() const override
        {
            return m_pGraphDefinition.IsLoaded() && m_pDataSet.IsLoaded();
        }

        inline Skeleton const* GetSkeleton() const 
        {
            KRG_ASSERT( IsValid() );
            return m_pDataSet->GetSkeleton();
        }

        inline GraphDefinition const* GetDefinition() const 
        {
            KRG_ASSERT( IsValid() );
            return m_pGraphDefinition.GetPtr();
        }

    protected:

        TResourcePtr<GraphDefinition>               m_pGraphDefinition = nullptr;
        TResourcePtr<GraphDataSet>                  m_pDataSet = nullptr;
    };
}