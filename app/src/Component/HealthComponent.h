#pragma once
#include "Component/Attack/AttackInfo.h"
#include "Component/AttributeComponent.h"
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include <functional>
#include <vector>

/// HPを持ち、ダメージを受けるコンポーネント
///
/// 誰に殴られたかは持たない。当たり判定から渡された攻撃情報を減らすだけなので、
/// プレイヤーにも敵にも同じものを付けられる
class HealthComponent : public Component {
  public:
    /// HPが尽きたときに呼ばれるコールバック
    using DeathCallback = std::function<void()>;

    /// attribute : 属性相性を見るための自分の属性（渡さなければ相性なしの等倍）
    explicit HealthComponent(const AttributeComponent *attribute = nullptr) : attribute_(attribute) {}

    void Init() override;

    void Update() override;

    /// 攻撃情報からダメージを受ける
    void TakeDamage(const AttackInfo &attackInfo);

    /// 攻撃力と属性からダメージを受ける
    void TakeDamage(float damage, AttributeType attackerAttribute = AttributeType::Red);

    /// HPが尽きたことを知りたい相手を追加する
    void AddDeathCallback(DeathCallback callback);

    float GetHp() const { return currentHp_; }
    float GetMaxHp() const { return maxHp_; }
    bool IsDead() const { return isDead_; }

    /// 被弾直後の無敵中か
    bool IsInvincible() const { return invincibilityTimer_ > 0.0f; }

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("Health");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    const AttributeComponent *attribute_ = nullptr; // 任意：属性相性を見るため

    // ==== 状態 ====
    float currentHp_ = 0.0f;           // 残りHP
    bool isDead_ = false;              // HPが尽きたか
    float invincibilityTimer_ = 0.0f;  // 被弾直後の無敵の残りフレーム
    std::vector<DeathCallback> deathCallbacks_;

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(float, maxHp_, 100.0f);
    // 被弾直後の無敵フレーム数（連続ヒットで一気に溶けないようにする）
    GameParameter(float, invincibilityFrame_, 30.0f);
};
