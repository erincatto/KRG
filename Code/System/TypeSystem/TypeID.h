#pragma once

#include "System/_Module/API.h"
#include "System/Types/StringID.h"
#include "System/Serialization/Serialization.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class KRG_SYSTEM_API TypeID
    {
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_ID ) );

    public:

        TypeID() {}
        TypeID( String const& type ) : m_ID( type ) {}
        TypeID( char const* pType ) : m_ID( pType ) {}
        TypeID( StringID ID ) : m_ID( ID ) {}
        TypeID( uint32_t ID ) : m_ID( ID ) {}

        KRG_FORCE_INLINE bool IsValid() const { return m_ID.IsValid(); }

        KRG_FORCE_INLINE operator uint32_t() const { return m_ID.GetID(); }
        KRG_FORCE_INLINE uint32_t GetID() const { return m_ID.GetID(); }
        KRG_FORCE_INLINE StringID ToStringID() const { return m_ID; }
        KRG_FORCE_INLINE char const* c_str() const { return m_ID.c_str(); }

    private:

        StringID m_ID;
    };
}

//-------------------------------------------------------------------------

namespace eastl
{
    template <>
    struct hash<KRG::TypeSystem::TypeID>
    {
        eastl_size_t operator()( KRG::TypeSystem::TypeID const& ID ) const 
        {
            return ID.GetID();
        }
    };
}