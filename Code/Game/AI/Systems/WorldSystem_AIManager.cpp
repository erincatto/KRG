#include "WorldSystem_AIManager.h"
#include "Game/AI/Components/Component_AI.h"
#include "Engine/AI/Components/Component_AISpawn.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityWorldUpdateContext.h"
#include "Engine/Entity/EntityMap.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Threading/TaskSystem.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    void AIManager::ShutdownSystem()
    {
        KRG_ASSERT( m_spawnPoints.empty() );
    }

    void AIManager::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pSpawnComponent = TryCast<AISpawnComponent>( pComponent ) )
        {
            m_spawnPoints.emplace_back( pSpawnComponent );
        }

        if ( auto pAIComponent = TryCast<AIComponent>( pComponent ) )
        {
            m_AIs.emplace_back( pAIComponent );
        }
    }

    void AIManager::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pSpawnComponent = TryCast<AISpawnComponent>( pComponent ) )
        {
            m_spawnPoints.erase_first_unsorted( pSpawnComponent );
        }

        if ( auto pAIComponent = TryCast<AIComponent>( pComponent ) )
        {
            m_AIs.erase_first( pAIComponent );
        }
    }

    //-------------------------------------------------------------------------

    void AIManager::UpdateSystem( EntityWorldUpdateContext const& ctx )
    {
        if ( ctx.IsGameWorld() && !m_hasSpawnedAI )
        {
            m_hasSpawnedAI = TrySpawnAI( ctx );
        }
    }

    bool AIManager::TrySpawnAI( EntityWorldUpdateContext const& ctx )
    {
        if ( m_spawnPoints.empty() )
        {
            return false;
        }

        auto pTypeRegistry = ctx.GetSystem<TypeSystem::TypeRegistry>();
        auto pTaskSystem = ctx.GetSystem<TaskSystem>();
        auto pPersistentMap = ctx.GetPersistentMap();

        //-------------------------------------------------------------------------

        for ( auto pSpawnPoint : m_spawnPoints )
        {
            pPersistentMap->AddEntityCollection( pTaskSystem, *pTypeRegistry, *pSpawnPoint->GetEntityCollectionDesc(), pSpawnPoint->GetWorldTransform() );
        }

        return true;
    }
}