#pragma once

#include "_Module/API.h"
#include "System/Core/Types/String.h"
#include "System/Core/Serialization/Serialization.h"

//-------------------------------------------------------------------------
// Resource Type ID
//-------------------------------------------------------------------------
// Unique ID for a resource - Used for resource look up and dependencies
// Resource type IDs are lowercase FourCCs i.e. can only contain lowercase ASCII letters and digits

namespace KRG
{
    class KRG_SYSTEM_RESOURCE_API ResourceTypeID
    {
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_ID ) );

    public:

        // Check if a given string is a valid resource type FourCC (i.e. [1:4] lowercase letters or digits)
        static bool IsValidResourceFourCC( char const* pStr );

        // Check if a given string is a valid resource type FourCC (i.e. [1:4] lowercase letters or digits)
        inline static bool IsValidResourceFourCC( String const& str ) { return IsValidResourceFourCC( str.c_str() ); }

        // Check if a given string is a valid resource type FourCC (i.e. [1:4] lowercase letters or digits)
        template<eastl_size_t S>
        inline static bool IsValidResourceFourCC( TInlineString<S> const& str ) { return IsValidResourceFourCC( str.c_str() ); }

        // Expensive verification to ensure that a resource type ID FourCC only contains uppercase or numeric chars
        static bool IsValidResourceFourCC( uint32 fourCC );

    public:

        inline ResourceTypeID() : m_ID( 0 ) {}
        inline ResourceTypeID( uint32 ID ) : m_ID( ID ) { KRG_ASSERT( IsValidResourceFourCC( m_ID ) ); }
        explicit ResourceTypeID( char const* pStr );
        inline explicit ResourceTypeID( String const& str ) : ResourceTypeID( str.c_str() ) {}

        inline bool IsValid() const { return m_ID != 0; }
        void Clear() { m_ID = 0; }

        inline operator uint32() const { return m_ID; }
        inline operator uint32&() { return m_ID; }

        //-------------------------------------------------------------------------

        inline void GetString( char outStr[5] ) const
        {
            KRG_ASSERT( IsValidResourceFourCC( m_ID ) );

            int32 idx = 0;

            outStr[idx] = (uint8) ( m_ID >> 24 );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = (uint8) ( ( m_ID & 0x00FF0000 ) >> 16 );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = (uint8) ( ( m_ID & 0x0000FF00 ) >> 8 );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = (uint8) ( ( m_ID & 0x000000FF ) );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = 0;
        }

        inline TInlineString<5> ToString() const
        {
            TInlineString<5> str;
            str.resize( 5 );
            GetString( str.data() );
            return str;
        }

    public:

        uint32                 m_ID;
    };
}

//-------------------------------------------------------------------------
// Support for THashMap

namespace eastl
{
    template <>
    struct hash<KRG::ResourceTypeID>
    {
        eastl_size_t operator()( KRG::ResourceTypeID const& ID ) const
        {
            return ( KRG::uint32 ) ID;
        }
    };
}