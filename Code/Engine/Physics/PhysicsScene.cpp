#include "PhysicsScene.h"
#include "PhysicsRagdoll.h"

#include <PxScene.h>

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    Scene::Scene( physx::PxScene* pScene )
        : m_pScene( pScene )
    {
        KRG_ASSERT( pScene != nullptr );
    }

    Scene::~Scene()
    {
        m_pScene->release();
        m_pScene = nullptr;
    }

    Ragdoll* Scene::CreateRagdoll( RagdollDefinition const* pDefinition, StringID const& profileID, uint64_t userID )
    {
        KRG_ASSERT( m_pScene != nullptr && pDefinition != nullptr );
        auto pRagdoll = KRG::New<Ragdoll>( &m_pScene->getPhysics(), pDefinition, profileID, userID );
        pRagdoll->AddToScene( m_pScene );
        return pRagdoll;
    }

    //-------------------------------------------------------------------------

    void Scene::AcquireReadLock()
    {
        m_pScene->lockRead();
        KRG_DEVELOPMENT_TOOLS_ONLY( ++m_readLockCount );
    }

    void Scene::ReleaseReadLock()
    {
        m_pScene->unlockRead();
        KRG_DEVELOPMENT_TOOLS_ONLY( --m_readLockCount );
    }

    void Scene::AcquireWriteLock()
    {
        m_pScene->lockWrite();
        KRG_DEVELOPMENT_TOOLS_ONLY( m_writeLockAcquired = true );
    }

    void Scene::ReleaseWriteLock()
    {
        m_pScene->unlockWrite();
        KRG_DEVELOPMENT_TOOLS_ONLY( m_writeLockAcquired = false );
    }
}