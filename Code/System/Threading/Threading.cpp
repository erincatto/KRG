#include "Threading.h"
#include "System/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Threading
    {
        static ThreadID g_mainThreadID = 0;

        //-------------------------------------------------------------------------

        bool IsMainThread()
        {
            return g_mainThreadID == GetCurrentThreadID();
        }

        void Initialize( char const* pMainThreadName )
        {
            g_mainThreadID = GetCurrentThreadID();
            SetCurrentThreadName( pMainThreadName );
        }

        void Shutdown()
        {
            g_mainThreadID = 0;
        }
    }
}