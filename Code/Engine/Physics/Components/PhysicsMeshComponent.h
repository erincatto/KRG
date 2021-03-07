#pragma once

#include "../_Module/API.h"
#include "PhysicsComponent.h"
#include "Engine/Physics/PhysicsMesh.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsMesh;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsMeshComponent final : public PhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

        friend class PhysicsWorldSystem;
    
    private:

        virtual bool HasValidPhysicsSetup() const override final;

        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final;

    protected:

        // Any additional physical materials needed for a triangle mesh
        EXPOSE TVector<StringID>                            m_additionalPhysicsMaterialIDs;

        // The collision mesh to load (can be either convex or concave)
        EXPOSE TResourcePtr<PhysicsMesh>                    m_pPhysicsMesh;
    };
}