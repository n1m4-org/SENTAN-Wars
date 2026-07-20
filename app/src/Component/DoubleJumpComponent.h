#pragma once
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Component.h"

class JumpComponent;

/// 二段ジャンプを解禁するコンポーネント
///
/// ジャンプの処理そのものはJumpComponentが持っているので、ここは回数を1回増やすだけ
/// 「装備している間だけ空中でもう1回跳べる」を、存在しているかどうかだけで表す
/// （このコンポーネントが無ければ回数は増えないので、判定を書く必要がない）
class DoubleJumpComponent : public Component {
  public:
    /// SENTANの装備時に生成されるため、依存はまとめて受け取る
    explicit DoubleJumpComponent(const SentanContext &context) : jump_(context.jump) {}

    /// 外したときに増やした分を戻す
    ~DoubleJumpComponent() override;

    void Init() override;

  private:
    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    JumpComponent *jump_ = nullptr; // ジャンプ回数を増やす対象

    // ==== 状態 ====
    bool isGranted_ = false; // 回数を増やし済みか（二重に増減させないため）
};
