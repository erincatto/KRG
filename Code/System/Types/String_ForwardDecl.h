#pragma once

//-------------------------------------------------------------------------

namespace eastl
{
    class allocator;

    template <typename T, typename Allocator>
    class basic_string;

    template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
    class fixed_string;
}

//-------------------------------------------------------------------------

namespace KRG
{
    using String = eastl::basic_string<char, eastl::allocator>;
    template<size_t S> using TInlineString = eastl::fixed_string<char, S, true, eastl::allocator>;
    using InlineString = eastl::fixed_string<char, 255, true, eastl::allocator>;
}