#pragma once

#include "System/_Module/API.h"
#include "System/Types/String_ForwardDecl.h"
#include "EASTL/hash_map.h"

//-------------------------------------------------------------------------
// String ID
//-------------------------------------------------------------------------
// Deterministic numeric ID generated from a string
// StringIDs are CASE-SENSITIVE!

namespace KRG
{
    class StringID_CustomAllocator;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_API StringID
    {

    public:

        using CachedString = eastl::basic_string<char, StringID_CustomAllocator>;
        using StringCache = eastl::hash_map<uint32_t, CachedString, eastl::hash<uint32_t>, eastl::equal_to<uint32_t>, StringID_CustomAllocator>;

        static StringCache const    s_stringCache;

    public:

        StringID() = default;
        explicit StringID( nullptr_t ) : m_ID( 0 ) {}
        explicit StringID( char const* pStr );
        explicit StringID( uint32_t ID ) : m_ID( ID ) {}
        explicit StringID( String const& str );

        inline bool IsValid() const { return m_ID != 0; }
        inline uint32_t GetID() const { return m_ID; }
        inline operator uint32_t() const { return m_ID; }

        inline void Clear() { m_ID = 0; }

        char const* c_str() const;

        inline bool operator==( StringID const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( StringID const& rhs ) const { return m_ID != rhs.m_ID; }

    private:

        uint32_t m_ID = 0;
    };
}

//-------------------------------------------------------------------------

namespace eastl
{
    template <>
    struct hash<KRG::StringID>
    {
        eastl_size_t operator()( KRG::StringID const& ID ) const { return (uint32_t) ID; }
    };
}