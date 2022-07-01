#pragma once

#include "System/Memory/Memory.h"
#include <EASTL/hash_map.h>

//-------------------------------------------------------------------------

namespace KRG
{
    template<typename K, typename V, typename H = eastl::hash<K>> using THashMap = eastl::hash_map<K, V, H>;
    template<typename K, typename V> using TPair = eastl::pair<K, V>;
}