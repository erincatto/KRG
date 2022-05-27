#pragma once

#include "_Module/API.h"
#include "Engine/Core/Entity/EntityComponent.h"
#include "System/Core/Types/UUID.h"
#include "PhysX.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    //-------------------------------------------------------------------------
    // Results from a given query
    //-------------------------------------------------------------------------

    template<int N>
    struct RayCastResultBuffer : public physx::PxHitBuffer<physx::PxRaycastHit>
    {
        RayCastResultBuffer() : physx::PxHitBuffer<physx::PxRaycastHit>( m_hits, N ) {}

        KRG_FORCE_INLINE Vector GetHitPosition() const
        {
            KRG_ASSERT( hasBlock );
            return FromPx( block.position );
        }

        Vector                  m_start;
        Vector                  m_end;
        physx::PxRaycastHit     m_hits[N];
    };

    using RayCastResults = RayCastResultBuffer<32>;

    //-------------------------------------------------------------------------

    template<int N>
    struct SweepResultBuffer : public physx::PxHitBuffer<physx::PxSweepHit>
    {
        friend class Scene;

    public:

        SweepResultBuffer() : physx::PxHitBuffer<physx::PxSweepHit>( m_hits, N ) {}

        KRG_FORCE_INLINE bool HadInitialOverlap() const { return hasBlock && block.hadInitialOverlap(); }
        KRG_FORCE_INLINE Vector const& GetShapePosition() const { return m_finalShapePosition; }
        KRG_FORCE_INLINE float GetSweptDistance() const { KRG_ASSERT( hasBlock ); return block.distance; }
        KRG_FORCE_INLINE float GetRemainingDistance() const { return m_remainingDistance; }

    private:

        inline void CalculateFinalShapePosition( float epsilon )
        {
            if ( !hasBlock )
            {
                m_finalShapePosition = m_sweepEnd;
                m_remainingDistance = 0.f;
            }
            else if ( block.hadInitialOverlap() )
            {
                m_finalShapePosition = m_sweepStart;
                m_remainingDistance = ( m_sweepEnd - m_sweepStart ).GetLength3();
            }
            else // Regular blocking hit
            {
                Vector sweepDirection;
                float originalSweepDistance = 0.0f;
                ( m_sweepEnd - m_sweepStart ).ToDirectionAndLength3( sweepDirection, originalSweepDistance );

                // Calculate the final shape position and remaining distance (including the epsilon)
                float const finalSweepDistance( Math::Max( 0.0f, ( block.distance - epsilon ) ) );
                m_finalShapePosition = Vector::MultiplyAdd( sweepDirection, Vector( finalSweepDistance ), m_sweepStart );
                m_remainingDistance = originalSweepDistance - finalSweepDistance;
            }
        }

    public:

        Quaternion              m_orientation = Quaternion::Identity;
        Vector                  m_sweepStart;
        Vector                  m_sweepEnd;
        Vector                  m_finalShapePosition;
        float                   m_remainingDistance;
        physx::PxSweepHit       m_hits[N];
    };

    using SweepResults = SweepResultBuffer<32>;

    //-------------------------------------------------------------------------
    
    template<int N>
    struct OverlapResultBuffer : public physx::PxHitBuffer<physx::PxOverlapHit>
    {
        OverlapResultBuffer() : physx::PxHitBuffer<physx::PxOverlapHit>( m_hits, N ) {}

        Vector                  m_position;
        Quaternion              m_orientation = Quaternion::Identity;
        physx::PxOverlapHit     m_hits[N];
    };

    using OverlapResults = OverlapResultBuffer<32>;

    //-------------------------------------------------------------------------
    // PhysX Query Filter
    //-------------------------------------------------------------------------
    // Helper to abstract some PhysX complexity, and to provide syntactic sugar
    // Currently word0 in the filter data is used to specify the layer to query against (all other words are currently left unset)
    // For the currently define layers in the engine, please refer to "PhysicsLayers.h"

    class QueryFilter final : public physx::PxQueryFilterCallback
    {
    public:

        enum class MobilityFilter
        {
            None,
            IgnoreStatic,
            IgnoreDynamic
        };

    public:

        // By default queries will collide against ALL static and dynamic actors 
        // The filter data is set by default to zero which skips the PxFilterData step - so will collide with all layers
        QueryFilter()
            : m_filterData( physx::PxQueryFilterData( physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER ) )
        {}

        // Create a query specifying the layer mask to use for filtering
        QueryFilter( uint32_t layerMask )
            : m_filterData( physx::PxQueryFilterData( physx::PxFilterData( layerMask, 0, 0, 0 ), physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER ) )
        {}

        // Layer Filtering
        //-------------------------------------------------------------------------
        // This allows you to specify what layers this query will be run against (e.g. collide with only the environment or with characters, etc... )

        void SetLayerMask( uint32_t layerMask )
        {
            m_filterData.data.word0 = layerMask;
        }

        // Mobility Filtering
        //-------------------------------------------------------------------------
        // By default queries will collide against both static and dynamic actors

        inline bool IsQueryingStaticActors()
        {
            return m_filterData.flags.isSet( physx::PxQueryFlag::eSTATIC );
        }

        inline bool IsQueryingDynamicActors()
        {
            return m_filterData.flags.isSet( physx::PxQueryFlag::eDYNAMIC );
        }

        inline void SetMobilityFilter( MobilityFilter filter )
        {
            switch ( filter )
            {
                case MobilityFilter::None:
                m_filterData.flags |= physx::PxQueryFlag::eSTATIC;
                m_filterData.flags |= ~physx::PxQueryFlag::eDYNAMIC;
                break;

                case MobilityFilter::IgnoreStatic:
                m_filterData.flags &= ~physx::PxQueryFlag::eSTATIC;
                m_filterData.flags |= physx::PxQueryFlag::eDYNAMIC;
                break;

                case MobilityFilter::IgnoreDynamic:
                m_filterData.flags |= physx::PxQueryFlag::eSTATIC;
                m_filterData.flags &= ~physx::PxQueryFlag::eDYNAMIC;
                break;
            }
        }

        // Ignore 
        //-------------------------------------------------------------------------

        void AddIgnoredComponent( ComponentID const& componentID )
        {
            KRG_ASSERT( componentID.IsValid() );
            m_ignoredComponents.emplace_back( componentID );
        }

        void AddIgnoredEntity( EntityID const& entityID )
        {
            KRG_ASSERT( entityID.IsValid() );
            m_ignoredEntities.emplace_back( entityID );
        }

    private:

        virtual physx::PxQueryHitType::Enum preFilter( physx::PxFilterData const& filterData, physx::PxShape const* pShape, physx::PxRigidActor const* pActor, physx::PxHitFlags& queryFlags ) override
        {
            for ( auto const& ignoredComponentID : m_ignoredComponents )
            {
                auto pOwnerComponent = reinterpret_cast<EntityComponent const*>( pActor->userData );
                if ( pOwnerComponent->GetID() == ignoredComponentID )
                {
                    return physx::PxQueryHitType::eNONE;
                }
            }

            //-------------------------------------------------------------------------

            for ( auto const& ignoredEntityID : m_ignoredEntities )
            {
                auto pOwnerComponent = reinterpret_cast<EntityComponent const*>( pActor->userData );
                if ( pOwnerComponent->GetEntityID() == ignoredEntityID )
                {
                    return physx::PxQueryHitType::eNONE;
                }
            }

            //-------------------------------------------------------------------------

            return physx::PxQueryHitType::eBLOCK;
        }

        virtual physx::PxQueryHitType::Enum postFilter( physx::PxFilterData const& filterData, physx::PxQueryHit const& hit ) override
        {
            KRG_UNREACHABLE_CODE(); // Not currently used
            return physx::PxQueryHitType::eBLOCK;
        }

    public:

        physx::PxQueryFilterData                            m_filterData;
        physx::PxHitFlags                                   m_hitFlags = physx::PxHitFlag::eDEFAULT | physx::PxHitFlag::eMTD;
        TInlineVector<ComponentID, 2>                       m_ignoredComponents;
        TInlineVector<EntityID, 2>                          m_ignoredEntities;
    };
}