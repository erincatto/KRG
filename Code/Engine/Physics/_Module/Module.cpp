#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "Engine/Render/RendererRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    namespace
    {
        constexpr static char const* const g_physicsMaterialDatabaseResourceID = "data://Physics/PhysicsMaterials.pmdb";
    }

    //-------------------------------------------------------------------------

    void EngineModule::GetListOfAllRequiredModuleResources( TVector<ResourceID>& outResourceIDs )
    {
        outResourceIDs.emplace_back( ResourceID( g_physicsMaterialDatabaseResourceID ) );
    }

    //-------------------------------------------------------------------------

    bool EngineModule::Initialize( ModuleContext& context, IniFile const& iniFile )
    {
        m_physicsSystem.Initialize();

        //-------------------------------------------------------------------------

        m_physicsMeshLoader.SetPhysics( &m_physicsSystem );
        context.RegisterResourceLoader( &m_physicsMeshLoader );

        m_physicsMaterialLoader.SetPhysics( &m_physicsSystem );
        context.RegisterResourceLoader( &m_physicsMaterialLoader );

        m_physicsRagdollLoader.SetPhysics( &m_physicsSystem );
        context.RegisterResourceLoader( &m_physicsRagdollLoader );

        //-------------------------------------------------------------------------

        context.RegisterSystem( m_physicsSystem );

        #if KRG_DEVELOPMENT_TOOLS
        m_physicsRenderer.Initialize( context.GetRenderDevice() );
        auto pRendererRegistry = context.GetSystem<Render::RendererRegistry>();
        KRG_ASSERT( pRendererRegistry != nullptr );
        pRendererRegistry->RegisterRenderer( &m_physicsRenderer );
        #endif

        //-------------------------------------------------------------------------

        return true;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        #if KRG_DEVELOPMENT_TOOLS
        auto pRendererRegistry = context.GetSystem<Render::RendererRegistry>();
        KRG_ASSERT( pRendererRegistry != nullptr );
        pRendererRegistry->UnregisterRenderer( &m_physicsRenderer );
        m_physicsRenderer.Shutdown();
        #endif

        context.UnregisterSystem( m_physicsSystem );

        //-------------------------------------------------------------------------

        context.UnregisterResourceLoader( &m_physicsRagdollLoader );
        m_physicsRagdollLoader.ClearPhysics();

        context.UnregisterResourceLoader( &m_physicsMaterialLoader );
        m_physicsMaterialLoader.ClearPhysics();

        context.UnregisterResourceLoader( &m_physicsMeshLoader );
        m_physicsMeshLoader.ClearPhysics();

        //-------------------------------------------------------------------------

        m_physicsSystem.Shutdown();
    }

    //-------------------------------------------------------------------------

    void EngineModule::LoadModuleResources( Resource::ResourceSystem& resourceSystem )
    {
        m_pPhysicMaterialDB = ResourceID( g_physicsMaterialDatabaseResourceID );
        KRG_ASSERT( m_pPhysicMaterialDB.IsValid() );
        resourceSystem.LoadResource( m_pPhysicMaterialDB );
    }

    bool EngineModule::VerifyModuleResourceLoadingComplete()
    {
        return m_pPhysicMaterialDB.IsLoaded() && m_pPhysicMaterialDB->IsValid();
    }

    void EngineModule::UnloadModuleResources( Resource::ResourceSystem& resourceSystem )
    {
        resourceSystem.UnloadResource( m_pPhysicMaterialDB );
    }
}