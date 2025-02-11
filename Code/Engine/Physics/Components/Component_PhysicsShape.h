#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Physics/PhysicsMaterial.h"
#include "Engine/Physics/PhysicsLayers.h"
#include "Engine/Entity/EntitySpatialComponent.h"
#include "System/Types/Event.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxShape;
    class PxRigidActor;
}

//-------------------------------------------------------------------------

namespace physx
{
    class PxActor;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    enum class ActorType
    {
        KRG_REGISTER_ENUM

        Static = 0,     // Immovable static object
        Dynamic,        // Movable, controlled by simulation
        Kinematic       // Movable, controlled by code
    };

    enum class ShapeType
    {
        KRG_REGISTER_ENUM

        SimulationAndQuery = 0,
        SimulationOnly,
        QueryOnly,
    };

    //-------------------------------------------------------------------------
    // Base class for all physics shape components
    //-------------------------------------------------------------------------
    // Each physics component will create a new actor in the scene
    // TODO: provide option to simple weld the shape from the component to its parent actor

    class KRG_ENGINE_API PhysicsShapeComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PhysicsShapeComponent );

        friend class PhysicsWorldSystem;

        static TEvent<PhysicsShapeComponent*> s_staticActorTransformChanged; // Fired whenever we change the scale for a physics shape

    public:

        inline static TEventHandle<PhysicsShapeComponent*> OnStaticActorTransformUpdated() { return s_staticActorTransformChanged; }

    public:

        inline PhysicsShapeComponent() = default;
        inline PhysicsShapeComponent( StringID name ) : SpatialEntityComponent( name ) {}

        inline ActorType GetActorType() const { return m_actorType; }
        inline ShapeType GetShapeType() const { return m_shapeType; }

        // Static API
        //-------------------------------------------------------------------------

        inline bool IsStatic() const { return m_actorType == ActorType::Static; }

        // Dynamic API
        //-------------------------------------------------------------------------

        inline bool IsDynamic() const { return m_actorType == ActorType::Dynamic; }

        void SetVelocity( Float3 newVelocity );

        // Kinematic API
        //-------------------------------------------------------------------------

        inline bool IsKinematic() const { return m_actorType == ActorType::Kinematic; }

        // This will set the component's world transform and teleport the physics actor to the desired location
        void TeleportTo( Transform const& newWorldTransform );

        // This will set the component's world transform and request that the kinematic actor be moved to the desired location, correctly interacting with other actors on it's path.
        // Note: the actual physics actor will only be moved during the next physics simulation step
        void MoveTo( Transform const& newWorldTransform );

    protected:

        // Check if the physics setup if valid for this component, will log any problems detected!
        virtual bool HasValidPhysicsSetup() const = 0;

        // Get physics materials for this component
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const = 0;

        // Update physics world position for this shape
        virtual void OnWorldTransformUpdated() override final;

    protected:

        // The type of physics actor for this component
        KRG_EXPOSE ActorType                            m_actorType = ActorType::Static;

        // How is this shape to be used
        KRG_EXPOSE ShapeType                            m_shapeType = ShapeType::SimulationAndQuery;

        // What layers does this shape belong to?
        KRG_EXPOSE TBitFlags<Layers>                    m_layers = Layers::Environment;

        // The mass of the shape, only relevant for dynamic actors
        KRG_EXPOSE float                                m_mass = 1.0f;

    private:

        physx::PxRigidActor*                            m_pPhysicsActor = nullptr;
        physx::PxShape*                                 m_pPhysicsShape = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        String                                          m_debugName; // Keep a debug name here since the physx SDK doesnt store the name data
        #endif
    };
}