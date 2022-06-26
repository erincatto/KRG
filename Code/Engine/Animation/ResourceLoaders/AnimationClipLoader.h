#pragma once

#include "Engine/_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClipLoader final : public Resource::ResourceLoader
    {
    public:

        AnimationClipLoader();
        ~AnimationClipLoader() { KRG_ASSERT( m_pTypeRegistry == nullptr ); }
        void SetTypeRegistryPtr( TypeSystem::TypeRegistry const* pTypeRegistry );
        void ClearTypeRegistryPtr() { m_pTypeRegistry = nullptr; }

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const override;
        virtual void UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const override;
        virtual Resource::InstallResult Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const override;

    private:

        TypeSystem::TypeRegistry const* m_pTypeRegistry = nullptr;
    };
}