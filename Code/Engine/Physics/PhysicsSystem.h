#pragma once

#include "Engine/_Module/API.h"
#include "PhysX.h"
#include "PhysicsMaterial.h"
#include "Engine/UpdateContext.h"
#include "System/Systems.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsMaterialDatabase;
    class SimulationFilter;
    class Scene;

    //-------------------------------------------------------------------------
    // Physics System
    //-------------------------------------------------------------------------
    // This is effectively a singleton since PhysX only allows a single instance of the foundation class

    class KRG_ENGINE_API PhysicsSystem : public ISystem
    {
        friend class ScopeLock;
        friend class PhysicsDebugView;

    public:

        KRG_SYSTEM_ID( PhysicsSystem );

    public:

        PhysicsSystem() = default;

        void Initialize();
        void Shutdown();
        void Update( UpdateContext& ctx );

        // PhysX Helpers
        //-------------------------------------------------------------------------

        // Get the created physx context
        inline physx::PxPhysics* GetPxPhysics() const { return m_pPhysics; }

        // Scene factory method - transfers ownership of the scene to the calling code
        Scene* CreateScene();

        // Physic Materials
        //-------------------------------------------------------------------------

        void FillMaterialDatabase( TVector<PhysicsMaterialSettings> const& materials );
        void ClearMaterialDatabase();

        physx::PxMaterial* GetDefaultMaterial() const { return m_pDefaultMaterial; }
        physx::PxMaterial* GetMaterial( StringID materialID ) const;

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        bool IsConnectedToPVD();
        void ConnectToPVD( Seconds timeToRecord = -1.0f );
        void DisconnectFromPVD();
        #endif

    private:

        void CreateSharedMeshes();
        void DestroySharedMeshes();

    private:

        physx::PxFoundation*                            m_pFoundation = nullptr;
        physx::PxPhysics*                               m_pPhysics = nullptr;
        physx::PxCooking*                               m_pCooking = nullptr;
        physx::PxCpuDispatcher*                         m_pDispatcher = nullptr;
        physx::PxAllocatorCallback*                     m_pAllocatorCallback = nullptr;
        physx::PxErrorCallback*                         m_pErrorCallback = nullptr;
        physx::PxSimulationEventCallback*               m_pEventCallbackHandler = nullptr;
        SimulationFilter*                               m_pSimulationFilterCallback = nullptr;

        THashMap<StringID, PhysicsMaterial>             m_materials;
        physx::PxMaterial*                              m_pDefaultMaterial = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        physx::PxPvd*                                   m_pPVD = nullptr;
        physx::PxPvdTransport*                          m_pPVDTransport = nullptr;
        Seconds                                         m_recordingTimeLeft = -1.0f;
        #endif
    };
}