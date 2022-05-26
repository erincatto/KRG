#ifdef _WIN32
#include "../FileSystemUtils.h"
#include "System/Core/Platform/PlatformHelpers_Win32.h"

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    Path GetCurrentProcessPath()
    {
        return Path( KRG::Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();
    }
}
#endif