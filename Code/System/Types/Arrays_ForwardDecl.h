#pragma once

//-------------------------------------------------------------------------

namespace eastl
{
    class allocator;

    template <typename T, typename Allocator>
    class vector;

    template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
    class fixed_vector;

    template <typename T, size_t N>
    struct array;
}

//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T> using TVector = eastl::vector<T, eastl::allocator>;
    template<typename T, eastl_size_t S> using TInlineVector = eastl::fixed_vector<T, S, true, eastl::allocator>;
    template<typename T, eastl_size_t S> using TArray = eastl::array<T, S>;
}