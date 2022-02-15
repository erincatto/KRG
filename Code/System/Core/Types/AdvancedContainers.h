#pragma once

#include <EASTL/queue.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/set.h>
#include <EASTL/vector_set.h>
#include <EASTL/hash_set.h>

//-------------------------------------------------------------------------
// Less commonly used container aliases
//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T> using TSortedVector = eastl::vector_set<T>;
    template<typename T> using TList = eastl::list<T>;
    template<typename T> using TSet = eastl::set<T>;
    template<typename T> using TStack = eastl::stack<T>;
    template<typename T> using TQueue = eastl::queue<T>;
    template<typename T> using TDeque = eastl::deque<T>;
    template<typename T> using TMultiSet = eastl::multiset<T>;
    template<typename V, typename H = eastl::hash<V>> using THashSet = eastl::hash_set<V, H>;
}