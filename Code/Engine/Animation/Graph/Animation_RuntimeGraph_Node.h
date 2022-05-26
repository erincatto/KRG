#pragma once

#include "Animation_RuntimeGraph_Contexts.h"
#include "Engine/Animation/AnimationSyncTrack.h"
#include "Engine/Animation/AnimationTarget.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClip;
    class GraphDataSet;
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API GraphNode
    {
        friend class PoseNode;
        friend class ValueNode;

    public:

        // This is the base for each node's individual settings
        // The settings are all shared for all graph instances since they are immutable, the nodes themselves contain the actual graph state
        struct KRG_ENGINE_ANIMATION_API Settings : public IRegisteredType
        {
            KRG_REGISTER_TYPE( Settings );

            enum class InitOptions
            {
                CreateNodeAndSetPointers,
                OnlySetPointers             // Needed when calling a parent 'InstantiateNode' function from a derived class
            };

        protected:

            template<typename T>
            KRG_FORCE_INLINE static void SetNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, GraphNodeIndex nodeIdx, T*& pTargetPtr )
            {
                KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                pTargetPtr = static_cast<T*>( nodePtrs[nodeIdx] );
            }

            template<typename T>
            KRG_FORCE_INLINE static void SetNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, GraphNodeIndex nodeIdx, T const*& pTargetPtr )
            {
                KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                pTargetPtr = static_cast<T const*>( nodePtrs[nodeIdx] );
            }

            template<typename T>
            KRG_FORCE_INLINE static void SetOptionalNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, GraphNodeIndex nodeIdx, T*& pTargetPtr )
            {
                if ( nodeIdx == InvalidIndex )
                {
                    pTargetPtr = nullptr;
                }
                else
                {
                    KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                    pTargetPtr = static_cast<T*>( nodePtrs[nodeIdx] );
                }
            }

            template<typename T>
            KRG_FORCE_INLINE static void SetOptionalNodePtrFromIndex( TVector<GraphNode*> const& nodePtrs, GraphNodeIndex nodeIdx, T const*& pTargetPtr )
            {
                if ( nodeIdx == InvalidIndex )
                {
                    pTargetPtr = nullptr;
                }
                else
                {
                    KRG_ASSERT( nodeIdx >= 0 && nodeIdx < nodePtrs.size() );
                    pTargetPtr = static_cast<T const*>( nodePtrs[nodeIdx] );
                }
            }

        public:

            virtual ~Settings() = default;

            // Factory method, will create the node instance and set all necessary node ptrs
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const = 0;

            // Serialization methods
            virtual void Load( cereal::BinaryInputArchive& archive ) { archive( m_nodeIdx ); }
            virtual void Save( cereal::BinaryOutputArchive& archive ) const { archive( m_nodeIdx ); }

        protected:

            template<typename T>
            KRG_FORCE_INLINE T* CreateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
            {
                T* pNode = static_cast<T*>( nodePtrs[m_nodeIdx] );

                if ( options == InitOptions::CreateNodeAndSetPointers )
                {
                    new ( pNode ) T();
                    pNode->m_pSettings = this;
                }

                return pNode;
            }

        public:

            GraphNodeIndex                      m_nodeIdx = InvalidIndex; // The index of this node in the graph, we currently only support graphs with max of 32k nodes
        };

    public:

        GraphNode() = default;
        virtual ~GraphNode();

        // Node state management
        //-------------------------------------------------------------------------

        virtual bool IsValid() const { return true; }
        virtual GraphValueType GetValueType() const = 0;
        inline GraphNodeIndex GetNodeIndex() const { return m_pSettings->m_nodeIdx; }

        inline bool IsInitialized() const { return m_initializationCount > 0; }
        virtual void Initialize( GraphContext& context );
        void Shutdown( GraphContext& context );

        // Update
        //-------------------------------------------------------------------------

        // Is this node active i.e. was it updated this frame
        KRG_FORCE_INLINE bool IsNodeActive( GraphContext& context ) const { return m_lastUpdateID == context.m_updateID; }

        // Was this node updated this frame, this is syntactic sugar for value nodes
        KRG_FORCE_INLINE bool WasUpdated( GraphContext& context ) const { return IsNodeActive( context ); }

        // Mark a node as being updated - value nodes will use this to cache values
        void MarkNodeActive( GraphContext& context );

    protected:

        virtual void InitializeInternal( GraphContext& context );
        virtual void ShutdownInternal( GraphContext& context );

        template<typename T>
        KRG_FORCE_INLINE typename T::Settings const* GetSettings() const
        {
            return static_cast<typename T::Settings const*>( m_pSettings );
        }

    private:

        Settings const*                 m_pSettings = nullptr;
        uint32                          m_lastUpdateID = 0xFFFFFFFF;
        uint32                          m_initializationCount = 0;
    };

    //-------------------------------------------------------------------------
    // Animation Nodes
    //-------------------------------------------------------------------------

    struct GraphPoseNodeResult
    {
        KRG_FORCE_INLINE bool HasRegisteredTasks() const { return m_taskIdx != InvalidIndex; }

    public:

        int8                            m_taskIdx = InvalidIndex;
        Transform                       m_rootMotionDelta = Transform::Identity;
        SampledEventRange               m_sampledEventRange;
    };

    #if KRG_DEVELOPMENT_TOOLS
    struct PoseNodeDebugInfo
    {
        int32                           m_loopCount = 0;
        Seconds                         m_duration = 0.0f;
        Percentage                      m_currentTime = 0.0f;       // Clamped percentage over the duration
        Percentage                      m_previousTime = 0.0f;      // Clamped percentage over the duration
        SyncTrack const*                m_pSyncTrack = nullptr;
        SyncTrackTime                   m_currentSyncTime;
    };
    #endif

    class KRG_ENGINE_ANIMATION_API PoseNode : public GraphNode
    {

    public:

        // Get internal animation state
        virtual SyncTrack const& GetSyncTrack() const = 0;
        inline int32 GetLoopCount() const { return m_loopCount; }
        inline Percentage const& GetPreviousTime() const { return m_previousTime; }
        inline Percentage const& GetCurrentTime() const { return m_currentTime; }
        inline Seconds GetDuration() const { return m_duration; }

        // Initialize an animation node with a specific start time
        void Initialize( GraphContext& context, SyncTrackTime const& initialTime = SyncTrackTime() );
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime );

        // Unsynchronized update
        virtual GraphPoseNodeResult Update( GraphContext& context ) = 0;

        // Synchronized update
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) = 0;

        // Deactivate a previous active branch, this is needed when trigger transitions
        virtual void DeactivateBranch( GraphContext& context ) { KRG_ASSERT( context.m_branchState == BranchState::Inactive && IsNodeActive( context ) ); }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        // Get the current state of the node
        PoseNodeDebugInfo GetDebugInfo() const;

        // Perform debug drawing for the pose node
        virtual void DrawDebug( Drawing::DrawContext& drawCtx, GraphContext& graphContext ) {}
        #endif

    private:

        virtual void Initialize( GraphContext& context ) override final { Initialize( context, SyncTrackTime() ); }
        virtual void InitializeInternal( GraphContext& context ) override final { Initialize( context, SyncTrackTime() ); }
        virtual GraphValueType GetValueType() const override final { return GraphValueType::Pose; }

    protected:

        int32                           m_loopCount = 0;
        Seconds                         m_duration = 0.0f;
        Percentage                      m_currentTime = 0.0f;       // Clamped percentage over the duration
        Percentage                      m_previousTime = 0.0f;      // Clamped percentage over the duration
    };

    //-------------------------------------------------------------------------
    // Value Nodes
    //-------------------------------------------------------------------------

    template<typename T> struct ValueTypeValidation { static GraphValueType const Type = GraphValueType::Unknown; };
    template<> struct ValueTypeValidation<bool> { static GraphValueType const Type = GraphValueType::Bool; };
    template<> struct ValueTypeValidation<StringID> { static GraphValueType const Type = GraphValueType::ID; };
    template<> struct ValueTypeValidation<int32> { static GraphValueType const Type = GraphValueType::Int; };
    template<> struct ValueTypeValidation<float> { static GraphValueType const Type = GraphValueType::Float; };
    template<> struct ValueTypeValidation<Vector> { static GraphValueType const Type = GraphValueType::Vector; };
    template<> struct ValueTypeValidation<Target> { static GraphValueType const Type = GraphValueType::Target; };
    template<> struct ValueTypeValidation<BoneMask const*> { static GraphValueType const Type = GraphValueType::BoneMask; };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ValueNode : public GraphNode
    {
    public:

        template<typename T>
        KRG_FORCE_INLINE T GetValue( GraphContext& context )
        {
            KRG_ASSERT( ValueTypeValidation<T>::Type == GetValueType() );
            T Value;
            GetValueInternal( context, &Value );
            return Value;
        }

        template<typename T>
        KRG_FORCE_INLINE void SetValue( GraphContext& context, T const& outValue )
        {
            KRG_ASSERT( ValueTypeValidation<T>::Type == GetValueType() );
            SetValueInternal( context, &outValue );
        }

    protected:

        virtual void GetValueInternal( GraphContext& context, void* pValue ) = 0;
        virtual void SetValueInternal( GraphContext& context, void const* pValue ) { KRG_ASSERT( false ); };
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API BoolValueNode : public ValueNode
    {
        virtual GraphValueType GetValueType() const override final { return GraphValueType::Bool; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API IDValueNode : public ValueNode
    {
        virtual GraphValueType GetValueType() const override final { return GraphValueType::ID; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API IntValueNode : public ValueNode
    {
        virtual GraphValueType GetValueType() const override final { return GraphValueType::Int; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API FloatValueNode : public ValueNode
    {
        virtual GraphValueType GetValueType() const override final { return GraphValueType::Float; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VectorValueNode : public ValueNode
    {
        virtual GraphValueType GetValueType() const override final { return GraphValueType::Vector; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API TargetValueNode : public ValueNode
    {
        virtual GraphValueType GetValueType() const override final { return GraphValueType::Target; }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API BoneMaskValueNode : public ValueNode
    {
        virtual GraphValueType GetValueType() const override final { return GraphValueType::BoneMask; }
    };
}

//-------------------------------------------------------------------------

#define KRG_SERIALIZE_GRAPHNODESETTINGS( BaseClassTypename, ... ) \
virtual void Load( cereal::BinaryInputArchive& archive ) override { BaseClassTypename::Load( archive ); archive( __VA_ARGS__ ); }\
virtual void Save( cereal::BinaryOutputArchive& archive ) const override { BaseClassTypename::Save( archive ); archive( __VA_ARGS__ ); }
