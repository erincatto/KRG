#include "WorldSystem_Navmesh.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Navmesh/Components/Component_Navmesh.h"
#include "Engine/Navmesh/NavmeshSystem.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityWorldUpdateContext.h"
#include "Engine/Render/RenderViewport.h"
#include "System/Profiling.h"
#include "System/Math/BoundingVolumes.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavmeshWorldSystem::InitializeSystem( SystemRegistry const& systemRegistry )
    {
        #if KRG_ENABLE_NAVPOWER
        auto pNavmeshSystem = systemRegistry.GetSystem<NavmeshSystem>();

        m_pInstance = bfx::SystemCreate( bfx::SystemParams( 2.0f, bfx::Z_UP ), pNavmeshSystem->m_pAllocator );
        bfx::SetCurrentInstance( nullptr );

        bfx::RegisterPlannerSystem( m_pInstance );
        bfx::SystemStart( m_pInstance );

        #if KRG_DEVELOPMENT_TOOLS
        bfx::SetRenderer( m_pInstance, &m_renderer );
        #endif
        #endif
    }

    void NavmeshWorldSystem::ShutdownSystem()
    {
        #if KRG_ENABLE_NAVPOWER
        KRG_ASSERT( m_registeredNavmeshes.empty() );

        #if KRG_DEVELOPMENT_TOOLS
        bfx::SetRenderer( m_pInstance, nullptr );
        #endif

        bfx::SystemStop( m_pInstance );
        bfx::SystemDestroy( m_pInstance );
        m_pInstance = nullptr;
        #endif
    }

    //-------------------------------------------------------------------------

    void NavmeshWorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pNavmeshComponent = TryCast<NavmeshComponent>( pComponent ) )
        {
            m_navmeshComponents.emplace_back( pNavmeshComponent );

            if ( pNavmeshComponent->HasNavmeshData() )
            {
                RegisterNavmesh( pNavmeshComponent );
            }
        }
    }

    void NavmeshWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pNavmeshComponent = TryCast<NavmeshComponent>( pComponent ) )
        {
            if ( pNavmeshComponent->HasNavmeshData() )
            {
                UnregisterNavmesh( pNavmeshComponent );
            }

            m_navmeshComponents.erase_first_unsorted( pNavmeshComponent );
        }
    }

    void NavmeshWorldSystem::RegisterNavmesh( NavmeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        #if KRG_ENABLE_NAVPOWER

        // Copy resource
        //-------------------------------------------------------------------------
        // NavPower operates on the resource in place so we need to make a copy

        NavmeshData const* pData = pComponent->m_pNavmeshData.GetPtr();
        KRG_ASSERT( pData != nullptr && pData->IsValid() );

        size_t const requiredMemory = sizeof( char ) * pData->GetGraphImage().size();
        char* pNavmesh = (char*) KRG::Alloc( requiredMemory );
        memcpy( pNavmesh, pData->GetGraphImage().data(), requiredMemory );

        // Add resource
        //-------------------------------------------------------------------------

        Transform const& componentWorldTransform = pComponent->GetWorldTransform();

        bfx::ResourceOffset offset;
        offset.m_positionOffset = ToBfx( componentWorldTransform.GetTranslation() );
        offset.m_rotationOffset = ToBfx( componentWorldTransform.GetRotation() );

        bfx::SpaceHandle space = bfx::GetDefaultSpaceHandle( m_pInstance );
        bfx::AddResource( space, pNavmesh, offset );

        // Add record
        m_registeredNavmeshes.emplace_back( RegisteredNavmesh( pComponent->GetID(), pNavmesh ) );

        #endif
    }

    void NavmeshWorldSystem::UnregisterNavmesh( NavmeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
        #if KRG_ENABLE_NAVPOWER

        for ( auto i = 0u; i < m_registeredNavmeshes.size(); i++ )
        {
            if ( pComponent->GetID() == m_registeredNavmeshes[i].m_componentID )
            {
                bfx::SpaceHandle space = bfx::GetDefaultSpaceHandle( m_pInstance );
                bfx::RemoveResource( space, m_registeredNavmeshes[i].m_pNavmesh );

                //-------------------------------------------------------------------------

                KRG::Free( m_registeredNavmeshes[i].m_pNavmesh );
                m_registeredNavmeshes.erase_unsorted( m_registeredNavmeshes.begin() + i );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
        #endif
    }

    //-------------------------------------------------------------------------

    void NavmeshWorldSystem::UpdateSystem( EntityWorldUpdateContext const& ctx )
    {
        #if KRG_ENABLE_NAVPOWER

        {
            KRG_PROFILE_SCOPE_NAVIGATION( "Navmesh Simulate" );
            bfx::SystemSimulate( m_pInstance, ctx.GetDeltaTime() );
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        {
            KRG_PROFILE_SCOPE_NAVIGATION( "Navmesh Debug Drawing" );
            m_renderer.SetDebugDrawingSystem( ctx.GetDebugDrawingSystem() );
            m_renderer.Reset();

            Render::Viewport const* pViewport = ctx.GetViewport();
        
            bfx::DrawCullParams cullParams;
            cullParams.m_cameraPos = ToBfx( pViewport->GetViewPosition() );
            cullParams.m_cameraDir = ToBfx( pViewport->GetViewForwardDirection() );
            cullParams.m_farClipDist = pViewport->GetViewVolume().GetDepthRange().m_end;
            cullParams.m_fov = pViewport->GetViewVolume().IsPerspective() ? pViewport->GetViewVolume().GetFOV().ToDegrees().ToFloat() : 0.0f;

            bfx::SystemDraw( m_pInstance, &cullParams );
        }
        #endif

        #endif
    }

    AABB NavmeshWorldSystem::GetNavmeshBounds( uint32_t layerIdx ) const
    {
        AABB bounds;

        #if KRG_ENABLE_NAVPOWER
        bfx::SpaceHandle spaceHandle = bfx::GetDefaultSpaceHandle( m_pInstance );
        bfx::Vector3 center, extents;
        if ( bfx::GetNavGraphBounds( spaceHandle, 1 << layerIdx, center, extents ) )
        {
            bounds.m_center = FromBfx( center );
            bounds.m_extents = FromBfx( extents );
        }
        #endif

        return bounds;
    }
}