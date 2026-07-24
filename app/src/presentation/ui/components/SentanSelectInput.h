#pragma once
#include <Input.h>


class SentanSelectInput
{
public:
    struct Result
    {
        uint32_t selectedIndex;
        bool isConfirmed;
    };

    SentanSelectInput();
    Result Update();

    /// カーソルの位置を外から動かす（選び終えたカードから移すときに使う）
    void SetSelectedIndex(uint32_t index) { selectedIndex_ = (index <= kMaxIndex_) ? index : kMaxIndex_; }

private:
    static constexpr uint32_t kMaxIndex_ = 2; // 選択肢の最大インデックス（0, 1, 2 の3つ）

    uint32_t selectedIndex_ = 0;
    Hagine::Input* pInput_ = nullptr;
};