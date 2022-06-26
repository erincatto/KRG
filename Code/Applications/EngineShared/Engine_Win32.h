#pragma once
#include "System/KRG.h"

//-------------------------------------------------------------------------

struct HWND__;

//-------------------------------------------------------------------------

namespace KRG
{
    intptr_t DefaultEngineWindowProcessor( class Engine* pEngine, HWND__* hWnd, uint32_t message, uintptr_t wParam, intptr_t lParam );
}