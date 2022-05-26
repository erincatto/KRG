#include "MathRandom.h"
#include "System/Core/Threading/Threading.h"
#include <random>

//-------------------------------------------------------------------------

namespace KRG::Math
{
    RNG::RNG()
    {
        m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
    }

    RNG::RNG( uint32 seed )
        : m_rng( seed )
    {
        KRG_ASSERT( seed != 0 );
    }

    //-------------------------------------------------------------------------

    namespace
    {
        RNG                 g_rng;
        Threading::Mutex    g_globalRandomMutex;
    }

    bool GetRandomBool()
    {
        Threading::ScopeLock lock( g_globalRandomMutex );
        return g_rng.GetUInt( 0, 1 ) == 1;
    }

    uint32 GetRandomUInt( uint32 min, uint32 max )
    {
        KRG_ASSERT( min < max );
        Threading::ScopeLock lock( g_globalRandomMutex );
        return g_rng.GetUInt( min, max );
    }

    int32 GetRandomInt( int32 min, int32 max )
    {
        KRG_ASSERT( min < max );

        uint32 const umax = max - min;
        int64 randomValue = GetRandomUInt( 0, umax );
        return static_cast<int32>( randomValue + min );
    }

    float GetRandomFloat( float min, float max )
    {
        Threading::ScopeLock lock( g_globalRandomMutex );
        return g_rng.GetFloat( min, max );
    }
}