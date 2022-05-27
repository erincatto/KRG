#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/KRG.h"
#include "System/Core/ThirdParty/pcg/include/pcg_random.hpp"

//-------------------------------------------------------------------------

namespace KRG::Math
{
    // Non-threadsafe random number generator based on PCG
    class KRG_SYSTEM_CORE_API RNG
    {

    public:

        RNG(); // Non-deterministic RNG
        RNG( uint32_t seed ); // Deterministic RNG

        inline uint32_t GetUInt( uint32_t min = 0, uint32_t max = 0xFFFFFFFF ) const
        {
            KRG_ASSERT( max > min );

            constexpr uint32_t const a = 0xFFFFFFFE;
            uint32_t const b = max - min;
            uint32_t const range = ( a <= b ) ? a : b;
            return min + m_rng( range + 1 );
        }

        inline float GetFloat( float min = 0.0f, float max = 1.0f ) const
        {
            KRG_ASSERT( max > min );
            return min + ( ( max - min ) * (float) ldexp( m_rng(), -32 ) );
        }

    private:

        mutable pcg32 m_rng;
    };

    // Threadsafe global versions - Use in non-performance critical code
    //-------------------------------------------------------------------------

    // Get a random unsigned integer value between [min, max]
    KRG_SYSTEM_CORE_API bool GetRandomBool();

    // Get a random unsigned integer value between [min, max]
    KRG_SYSTEM_CORE_API uint32_t GetRandomUInt( uint32_t min = 0, uint32_t max = UINT_MAX );

    // Get a random signed integer value between [min, max]
    KRG_SYSTEM_CORE_API int32_t GetRandomInt( int32_t min = INT_MIN, int32_t max = INT_MAX );

    // Get a random float value between [min, max]
    KRG_SYSTEM_CORE_API float GetRandomFloat( float min = 0, float max = 1.0f );
}