#include "ResourceLoader_PhysicsRagdoll.h"
#include "Engine/Physics/PhysicsRagdoll.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    RagdollLoader::RagdollLoader()
    {
        m_loadableTypes.push_back( RagdollDefinition::GetStaticResourceTypeID() );
    }

    void RagdollLoader::SetPhysics( PhysicsSystem* pPhysicsSystem )
    {
        KRG_ASSERT( pPhysicsSystem != nullptr && m_pPhysicsSystem == nullptr );
        m_pPhysicsSystem = pPhysicsSystem;
    }

    bool RagdollLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );
        RagdollDefinition* pRagdoll = KRG::New<RagdollDefinition>();
        archive >> *pRagdoll;

        pResourceRecord->SetResourceData( pRagdoll );
        return pRagdoll->IsValid();
    }

    Resource::InstallResult RagdollLoader::Install( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        RagdollDefinition* pRagdoll = pResourceRecord->GetResourceData<RagdollDefinition>();
        pRagdoll->m_pSkeleton = GetInstallDependency( installDependencies, pRagdoll->m_pSkeleton.GetResourceID() );
        pRagdoll->CreateRuntimeData();
        return Resource::InstallResult::Succeeded;
    }
}