#ifdef _WIN32
#include "../UUID.h"
#include <Objbase.h>

//-------------------------------------------------------------------------

namespace KRG
{
    UUID UUID::GenerateID()
    {
        UUID newID;
        static_assert( sizeof( GUID ) == sizeof( UUID ), "Size mismatch for KRG GUID vs Win32 GUID" );
        CoCreateGuid( (GUID*) &newID );
        return newID;
    }

    //-------------------------------------------------------------------------

    namespace StringUtils
    {
        int32 CompareInsensitive( char const* pStr0, char const* pStr1 )
        {
            return _stricmp( pStr0, pStr1 );
        }

        int32 CompareInsensitive( char const* pStr0, char const* pStr1, size_t n )
        {
            return _strnicmp( pStr0, pStr1, n );
        }
    }
}
#endif