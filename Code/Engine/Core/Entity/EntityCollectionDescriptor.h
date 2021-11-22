#pragma once
#include "Engine/Core/Entity/EntityDescriptors.h"
#include "System/Resource/IResource.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    //-------------------------------------------------------------------------
    // Entity Collection Template
    //-------------------------------------------------------------------------
    // This is a read-only resource that contains a collection of serialized entity descriptors
    // We used this to instantiate a collection of entities
    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityCollectionDescriptor : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'EC', "Entity Collection" );
        KRG_SERIALIZE_MEMBERS( m_entityDescriptors, m_entityLookupMap, m_entitySpatialAttachmentInfo );

        friend class EntityCollection;
        friend class EntityCollectionLoader;

    public:

        struct SearchResult
        {
            EntityDescriptor*       m_pEntity = nullptr;
            ComponentDescriptor*    m_pComponent = nullptr;
        };

    protected:

        struct SpatialAttachmentInfo
        {
            KRG_SERIALIZE_MEMBERS( m_entityIdx, m_parentEntityIdx );

            int32                                                         m_entityIdx = InvalidIndex;
            int32                                                         m_parentEntityIdx = InvalidIndex;
        };

    public:

        virtual bool IsValid() const override
        {
            return m_entityDescriptors.size() > 0 && m_entityDescriptors.size() == m_entityLookupMap.size();
        }

        // Template Creation
        //-------------------------------------------------------------------------

        void Reserve( int32 numEntities );

        inline void AddEntity( EntityDescriptor const& entityDesc )
        {
            KRG_ASSERT( entityDesc.IsValid() );
            m_entityLookupMap.insert( TPair<UUID, int32>( entityDesc.m_ID, (int32) m_entityDescriptors.size() ) );
            m_entityDescriptors.emplace_back( entityDesc );
        }

        void GenerateSpatialAttachmentInfo();

        void Clear() { m_entityDescriptors.clear(); m_entityLookupMap.clear(); m_entitySpatialAttachmentInfo.clear(); }

        // Entity Access
        //-------------------------------------------------------------------------

        inline int32 GetNumEntityDescriptors() const
        {
            return (int32) m_entityDescriptors.size();
        }

        inline TVector<EntityDescriptor> const& GetEntityDescriptors() const
        { 
            return m_entityDescriptors; 
        }

        inline EntityDescriptor const* FindEntityDescriptor( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            auto const foundEntityIter = m_entityLookupMap.find( entityID );
            if ( foundEntityIter != m_entityLookupMap.end() )
            {
                return &m_entityDescriptors[foundEntityIter->second];
            }
            else
            {
                return nullptr;
            }
        }

        inline int32 FindEntityIndex( UUID const& entityID ) const
        {
            KRG_ASSERT( entityID.IsValid() );

            auto const foundEntityIter = m_entityLookupMap.find( entityID );
            if ( foundEntityIter != m_entityLookupMap.end() )
            {
                return foundEntityIter->second;
            }
            else
            {
                return InvalidIndex;
            }
        }

        // Component Access
        //-------------------------------------------------------------------------

        TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes = true );

        inline TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes = true ) const
        {
            return const_cast<EntityCollectionDescriptor*>( this )->GetComponentsOfType( typeRegistry, typeID, allowDerivedTypes );
        }

        template<typename T>
        inline TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, bool allowDerivedTypes = true )
        {
            return GetComponentsOfType( typeRegistry, T::GetStaticTypeID(), allowDerivedTypes );
        }

        template<typename T>
        inline TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, bool allowDerivedTypes = true ) const
        {
            return const_cast<EntityCollectionDescriptor*>( this )->GetComponentsOfType( typeRegistry, T::GetStaticTypeID(), allowDerivedTypes );
        }

        bool HasComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes = true ) const { return !const_cast<EntityCollectionDescriptor*>( this )->GetComponentsOfType( typeRegistry, typeID, allowDerivedTypes ).empty(); }

        template<typename T>
        inline bool HasComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, bool allowDerivedTypes = true ) const
        {
            return HasComponentsOfType( typeRegistry, T::GetStaticTypeID(), allowDerivedTypes );
        }

    protected:

        TVector<EntityDescriptor>                                   m_entityDescriptors;
        THashMap<UUID, int32>                                       m_entityLookupMap;
        TVector<SpatialAttachmentInfo>                              m_entitySpatialAttachmentInfo;
    };
}