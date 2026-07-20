#pragma once
#include "Component/Component.h"

/// 攻撃系コンポーネントが共有する状態
/// 攻撃コンポーネント同士は互いを知らないので、「今どれか攻撃中か」はここに集める
class AttackStateComponent : public Component {
  public:
    /// 新しく攻撃を始められるか（他の攻撃が出ていないか）
    bool CanAttack() const { return !isAttacking_; }

    /// どれかの攻撃が出ている最中か
    bool IsAttacking() const { return isAttacking_; }

    /// 攻撃の開始を通知する
    void BeginAttack() { isAttacking_ = true; }

    /// 攻撃の終了を通知する
    void EndAttack() { isAttacking_ = false; }

  private:
    // どれかの攻撃が出ている最中か
    bool isAttacking_ = false;
};
