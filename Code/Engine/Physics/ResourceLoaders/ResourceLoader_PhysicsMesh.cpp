#include "ResourceLoader_PhysicsMesh.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysicsMesh.h"
#include "System/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------
namespace KRG::Physics
{
    class PhysXSerializedInputData : public physx::PxInputStream
    {
    public:

        PhysXSerializedInputData( TVector<uint8_t> const& buffer ) : m_buffer( buffer ) {}

    private:

        virtual PxU32 read( void* dest, PxU32 count ) override final
        {
            KRG_ASSERT( dest != nullptr );
            memcpy( dest, &m_buffer[m_readByteIdx], count );
            m_readByteIdx += count;
            return count;
        }

    private:

        TVector<uint8_t> const&    m_buffer;
        size_t                  m_readByteIdx = 0;
    };

    //-------------------------------------------------------------------------

    PhysicsMeshLoader::PhysicsMeshLoader()
    {
        m_loadableTypes.push_back( PhysicsMesh::GetStaticResourceTypeID() );
    }

    void PhysicsMeshLoader::SetPhysicsSystemPtr( PhysicsSystem* pPhysicsSystem )
    {
        KRG_ASSERT( pPhysicsSystem != nullptr && m_pPhysicsSystem == nullptr );
        m_pPhysicsSystem = pPhysicsSystem;
    }

    bool PhysicsMeshLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        // Create mesh resource
        PhysicsMesh* pPhysicsMesh = KRG::New<PhysicsMesh>();
        archive >> *pPhysicsMesh;

        // Deserialize cooked mesh data
        TVector<uint8_t> cookedMeshData;
        archive >> cookedMeshData;

        PhysXSerializedInputData cooked( cookedMeshData );
        {
            if ( pPhysicsMesh->m_isConvexMesh )
            {
                pPhysicsMesh->m_pMesh = m_pPhysicsSystem->GetPxPhysics()->createConvexMesh( cooked );
            }
            else
            {
                pPhysicsMesh->m_pMesh = m_pPhysicsSystem->GetPxPhysics()->createTriangleMesh( cooked );
            }
        }

        pResourceRecord->SetResourceData( pPhysicsMesh );
        return true;
    }

    void PhysicsMeshLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        PhysicsMesh* pPhysicsGeometry = pResourceRecord->GetResourceData<PhysicsMesh>();
        if ( pPhysicsGeometry != nullptr )
        {
            if ( pPhysicsGeometry->m_pMesh != nullptr )
            {
                pPhysicsGeometry->m_pMesh->release();
                pPhysicsGeometry->m_pMesh = nullptr;
            }
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}