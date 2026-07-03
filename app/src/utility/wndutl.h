#pragma once
#include <Windows.h>
#include <WinApp.h>

namespace utl::window
{
    inline POINT GetCenterOfWindow()
    {
        RECT windowRect;
        GetClientRect(Hagine::WinApp::GetInstance()->GetHwnd(), &windowRect);
        POINT center = {
            (windowRect.left + windowRect.right) / 2,
            (windowRect.top + windowRect.bottom) / 2
        };
        return center;
    }
}