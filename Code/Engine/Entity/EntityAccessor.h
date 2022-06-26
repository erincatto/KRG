#pragma once
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityComponent.h"
#include "Engine/Entity/EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Template Entity Model Accessor
    //-------------------------------------------------------------------------
    // This is a helper to access private data for entities, entity components and entity systems in the tools
    // Prevents bleeding tools only code into runtime code base
    //
    // Example Usage:
    // 
    // template<>
    // struct TEntityToolAccessor<Physics::PhysicsMeshComponent>
    // {
    //     TEntityToolAccessor( Physics::PhysicsMeshComponent* pType )
    //         : m_pType( pType )
    //     {}
    // 
    //     inline ResourceID const& GetMeshResourceID() { return m_pType->m_pPhysicsMesh.GetResourceID(); }
    // 
    // protected:
    // 
    //     Physics::PhysicsMeshComponent* m_pType = nullptr;
    // };

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    template<typename T>
    struct TEntityAccessor
    {
        static_assert( std::is_base_of<KRG::Entity, T>::value || std::is_base_of<KRG::EntityComponent, T>::value || std::is_base_of<KRG::EntitySystem, T>::value, "Invalid specialization - Only allowed to access Entity/Component/System." );
    };
    #endif
}