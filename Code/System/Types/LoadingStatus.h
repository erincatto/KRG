#pragma once

//-------------------------------------------------------------------------

namespace KRG
{
    enum class LoadingStatus
    {
        Unloaded = 0,
        Loading,
        Loaded,
        Unloading,
        Failed,
    };
}