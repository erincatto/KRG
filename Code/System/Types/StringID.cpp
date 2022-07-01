#include "StringID.h"
#include "System/Memory/Memory.h"
#include "System/Algorithm/Hash.h"
#include "System/Threading/Threading.h"
#include "String.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class StringID_CustomAllocator
    {
    public:

        EASTL_ALLOCATOR_EXPLICIT StringID_CustomAllocator( const char* pName = EASTL_NAME_VAL( EASTL_ALLOCATOR_DEFAULT_NAME ) ) {}
        StringID_CustomAllocator( const StringID_CustomAllocator& x ) {}
        StringID_CustomAllocator( const StringID_CustomAllocator& x, const char* pName ) {}
        StringID_CustomAllocator& operator=( const StringID_CustomAllocator& x ) { return *this; }
        const char* get_name() const { return "StringID"; }
        void set_name( const char* pName ) {}

        void* allocate( size_t n, int flags = 0 )
        {
            return allocate( n, EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT, 0, flags );
        }

        void* allocate( size_t n, size_t alignment, size_t offset, int flags = 0 )
        {
            size_t adjustedAlignment = ( alignment > EA_PLATFORM_PTR_SIZE ) ? alignment : EA_PLATFORM_PTR_SIZE;

            void* p = new char[n + adjustedAlignment + EA_PLATFORM_PTR_SIZE];
            void* pPlusPointerSize = (void*) ( (uintptr_t) p + EA_PLATFORM_PTR_SIZE );
            void* pAligned = (void*) ( ( (uintptr_t) pPlusPointerSize + adjustedAlignment - 1 ) & ~( adjustedAlignment - 1 ) );

            void** pStoredPtr = (void**) pAligned - 1;
            EASTL_ASSERT( pStoredPtr >= p );
            *( pStoredPtr ) = p;

            EASTL_ASSERT( ( (size_t) pAligned & ~( alignment - 1 ) ) == (size_t) pAligned );
            return pAligned;
        }

        void deallocate( void* p, size_t n )
        {
            if ( p != nullptr )
            {
                void* pOriginalAllocation = *( (void**) p - 1 );
                delete[]( char* )pOriginalAllocation;
            }
        }
    };

    inline bool operator==( const StringID_CustomAllocator&, const StringID_CustomAllocator& ) { return true; }
    inline bool operator!=( const StringID_CustomAllocator&, const StringID_CustomAllocator& ) { return false; }

    //-------------------------------------------------------------------------

    StringID::StringCache const StringID::s_stringCache;
    Threading::Mutex g_stringCacheMutex;

    //-------------------------------------------------------------------------

    StringID::StringID( char const* pStr )
    {
        if ( pStr != nullptr )
        {
            m_ID = Hash::GetHash32( pStr );

            // Cache the string
            Threading::ScopeLock lock( g_stringCacheMutex );
            auto iter = StringID::s_stringCache.find( m_ID );
            if ( iter == StringID::s_stringCache.end() )
            {
                auto& nonConstStringMap = const_cast<StringID::StringCache&>( StringID::s_stringCache );
                nonConstStringMap[m_ID] = StringID::CachedString( pStr );
            }
        }
    }

    StringID::StringID( String const& str )
        : StringID( str.c_str() )
    {}

    char const* StringID::c_str() const
    {
        if ( m_ID == 0 )
        {
            return nullptr;
        }

        {
            // Get cached string
            Threading::ScopeLock lock( g_stringCacheMutex );
            auto iter = StringID::s_stringCache.find( m_ID );
            if ( iter != StringID::s_stringCache.end() )
            {
                return iter->second.c_str();
            }
        }

        // ID likely directly created via uint32_t
        return nullptr;
    }
}