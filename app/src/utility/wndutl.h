#pragma once
#include <Windows.h>

namespace utl::window
{
    // ウィンドウのクライアント中心座標（クライアント座標系）を返す
    inline POINT GetCenterOfWindow(HWND hwnd)
    {
        RECT windowRect;
        GetClientRect(hwnd, &windowRect);
        POINT center = {
            (windowRect.left + windowRect.right) / 2,
            (windowRect.top + windowRect.bottom) / 2
        };
        return center;
    }
}
