#include "DoubleJumpComponent.h"
#include "Component/JumpComponent.h"

void DoubleJumpComponent::Init() {
    // 空中で跳べる回数を1回増やす（これで接地1回＋空中1回＝二段ジャンプになる）
    if (!jump_ || isGranted_) {
        return;
    }
    jump_->AddExtraJump(1);
    isGranted_ = true;
}

DoubleJumpComponent::~DoubleJumpComponent() {
    // SENTANを外したときに、増やした回数を元へ戻す
    if (jump_ && isGranted_) {
        jump_->AddExtraJump(-1);
    }
}
