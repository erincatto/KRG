#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Entity/EntityDescriptors.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_ENGINE_API EntityCollectionLoader : public Resource::ResourceLoader
    {
    public:

        EntityCollectionLoader();
        ~EntityCollectionLoader() { KRG_ASSERT( m_pTypeRegistry == nullptr ); }

        void SetTypeRegistryPtr( TypeSystem::TypeRegistry const* pTypeRegistry );
        inline void ClearTypeRegistryPtr() { m_pTypeRegistry = nullptr; }

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryInputArchive& archive ) const final;

    private:

        TypeSystem::TypeRegistry const* m_pTypeRegistry;
    };
}