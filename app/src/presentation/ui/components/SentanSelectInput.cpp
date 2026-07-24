#include "SentanSelectInput.h"
#include <algorithm>



SentanSelectInput::SentanSelectInput()
{
    pInput_ = Hagine::Input::GetInstance();
}

SentanSelectInput::Result SentanSelectInput::Update()
{
    /// インデックスの更新
    if (pInput_->TriggerKey(DIK_LEFTARROW) || pInput_->TriggerKey(DIK_A))
    {
        if (selectedIndex_ > 0)
        {
            --selectedIndex_;
        }
    }
    else if (pInput_->TriggerKey(DIK_RIGHTARROW) || pInput_->TriggerKey(DIK_D))
    {
        if (selectedIndex_ < kMaxIndex_)
        {
            ++selectedIndex_;
        }
    }

    // 決定はSpace（Fはポータルに入るのに使うので分けている）
    bool isConfirmed = pInput_->TriggerKey(DIK_SPACE);

    return { selectedIndex_, isConfirmed };
}
