#pragma once

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceID;
    namespace Resource { class ResourceSystem; class ResourceDatabase; }
    namespace TypeSystem { class TypeRegistry; }

    //-------------------------------------------------------------------------

    class ToolsContext
    {
    public:

        inline bool IsValid() const { return m_pTypeRegistry != nullptr && m_pResourceDatabase != nullptr && m_pResourceSystem != nullptr; }

        virtual void TryOpenResource( ResourceID const& resourceID ) const = 0;

    public:

        TypeSystem::TypeRegistry const*                     m_pTypeRegistry = nullptr;
        Resource::ResourceDatabase const*                   m_pResourceDatabase = nullptr;
        Resource::ResourceSystem*                           m_pResourceSystem = nullptr;
    };
}