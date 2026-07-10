#pragma once
#include <WinUser.h>

class ScopedCursorHidden
{
public:
    inline ScopedCursorHidden()
    {
        // カーソルを非表示にする
        ShowCursor(FALSE);
    }

    inline ~ScopedCursorHidden()
    {
        // カーソルを表示する
        ShowCursor(TRUE);
    }
};