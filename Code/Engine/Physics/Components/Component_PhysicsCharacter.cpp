#include "Component_PhysicsCharacter.h"
#include "Engine/Physics/PhysX.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    void CharacterComponent::Initialize()
    {
        SpatialEntityComponent::Initialize();

        // Ensure uniform scaling!
        if ( !GetWorldTransform().HasUniformScale() )
        {
            KRG_LOG_ERROR( "Physics", "Characters are not allowed to be non-uniformly scale - Scale was reset!" );
            auto WT = GetWorldTransform();
            WT.SetScale( 1.0f );
            SetWorldTransform( WT );
        }

        m_capsuleWorldTransform = CalculateCapsuleTransformFromWorldTransform( GetWorldTransform() );
    }

    bool CharacterComponent::HasValidPhysicsSetup() const
    {
        if ( m_radius <= 0 || m_cylinderPortionHalfHeight <= 0 )
        {
            KRG_LOG_ERROR( "Physics", "Invalid radius or half height on Physics Capsule Component: %s (%u). Negative or zero values are not allowed!", GetName().c_str(), GetID() );
            return false;
        }

        return true;
    }

    void CharacterComponent::OnWorldTransformUpdated()
    {
        // Ensure uniform scaling!
        if ( !GetWorldTransform().HasUniformScale() )
        {
            KRG_LOG_ERROR( "Physics", "Characters are not allowed to be non-uniformly scale - Scale was reset!" );
            auto WT = GetWorldTransform();
            WT.SetScale( 1.0f );
            SetWorldTransform( WT );
            return; // return here since this callback will be called via the set world transform
        }

        //-------------------------------------------------------------------------

        m_capsuleWorldTransform = CalculateCapsuleTransformFromWorldTransform( GetWorldTransform() );
        m_linearVelocity = Vector::Zero;

        if ( m_pPhysicsActor != nullptr )
        {
            // Teleport kinematic body
            auto physicsScene = m_pPhysicsActor->getScene();
            physicsScene->lockWrite();
            auto pKinematicActor = m_pPhysicsActor->is<physx::PxRigidDynamic>();
            KRG_ASSERT( pKinematicActor->getRigidBodyFlags().isSet( physx::PxRigidBodyFlag::eKINEMATIC ) );
            pKinematicActor->setGlobalPose( ToPx( m_capsuleWorldTransform ) );
            physicsScene->unlockWrite();
        }
    }

    void CharacterComponent::MoveCharacter( Seconds const deltaTime, Transform const& newWorldTransform )
    {
        KRG_ASSERT( deltaTime > 0.0f );

        Vector const deltaTranslation = newWorldTransform.GetTranslation() - GetPosition();
        m_linearVelocity = deltaTranslation / deltaTime;

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pPhysicsActor != nullptr  );
        SetWorldTransformDirectly( newWorldTransform, false ); // Do not fire callback as we dont want to teleport the character
        m_capsuleWorldTransform = CalculateCapsuleTransformFromWorldTransform( GetWorldTransform() );

        // Request the kinematic body be moved by the physics simulation
        auto physicsScene = m_pPhysicsActor->getScene();
        physicsScene->lockWrite();
        auto pKinematicActor = m_pPhysicsActor->is<physx::PxRigidDynamic>();
        KRG_ASSERT( pKinematicActor->getRigidBodyFlags().isSet( physx::PxRigidBodyFlag::eKINEMATIC ) );
        pKinematicActor->setKinematicTarget( ToPx( m_capsuleWorldTransform ) );
        physicsScene->unlockWrite();
    }
}