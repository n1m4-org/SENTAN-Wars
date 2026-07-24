#pragma once
#include "3d/Object/Base/BaseObject.h"
#include "Character/Player/Sentan/Sentan.h"
#include "Character/Player/Sentan/SentanDefinition.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Component/BodyColliderComponent.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"
#include <memory>
#include <vector>

/// プレイヤーの武器本体
/// Forkは常に1本だけで準備フェーズで取得したSENTAN(最大2つ)はこのForkにくっつく
/// 装着したSENTANの位置はForkが決めるので、SENTAN側は位置を持たない
class Fork : public Hagine::BaseObject {
  public:
    ~Fork();

    void Init(const std::string className) override;

    void Update() override;

    void Draw(const Hagine::ViewProjection &viewProjection) override;

    /// SENTANは最大2つまで（選ばせる側が上限を知るため公開する）
    static constexpr size_t kMaxSentanCount = 2;

    /// SENTANをくっつける（最大2つまで）
    Sentan *AttachSentan(SentanId id);

    /// くっついているSENTANを全部外す
    /// 選び直しのたびに丸ごと付け替えるため、1本ずつ外す口は用意していない
    void DetachAllSentan();

    /// 攻撃モーション用の追加オフセット（構え位置からのズレ）
    /// 攻撃コンポーネントが毎フレーム設定する
    void SetMotionOffset(const Hagine::Vector3 &offset) { motionOffset_ = offset; }

    /// 攻撃モーション用の追加回転（構えの回転からのズレ・ラジアン）
    /// 攻撃コンポーネントが毎フレーム設定する
    void SetMotionRotation(const Hagine::Vector3 &rotation) { motionRotation_ = rotation; }

    /// 武器自身の軸まわりの自転（ラジアン）
    /// ドリルのように回すためのもので、倒した向きに追従して回る
    /// （オイラー角へ足すとワールド軸で回ってしまうため、別に持って合成する）
    void SetMotionSpin(float spin) { motionSpin_ = spin; }

    /// くっついているSENTANの数
    size_t GetSentanCount() const { return sentans_.size(); }

    /// 攻撃の当たり判定の有効/無効を切り替える
    /// Fork自身と、くっついているSENTAN全部をまとめて扱う
    /// （呼ぶ側はSENTANが何本あるかも、何のSENTANかも知らなくてよい）
    /// hit は開けている間の攻撃力と、判定を広げる倍率
    void SetAttackColliderEnabled(bool enabled, const HitWindow &hit = {});

    /// 攻撃者（ダメージを与えた相手に伝えるためのもの）
    void SetAttacker(Hagine::BaseObject *attacker) { attacker_ = attacker; }

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("Fork");

    // ==== 構え位置（プレイヤーからの相対・デバッグ調整） ====
    GameParameter(Hagine::Vector3, basePosition_, (Hagine::Vector3{1.5f, 1.0f, 0.5f})); // 位置
    GameParameter(Hagine::Vector3, baseRotation_, (Hagine::Vector3{0.0f, 0.0f, 0.0f})); // 回転
    GameParameter(Hagine::Vector3, baseScale_, (Hagine::Vector3{1.0f, 1.0f, 1.0f}));    // スケール

    // ==== SENTANの装着位置（Fork基準・デバッグ調整） ====
    GameParameter(Hagine::Vector3, sentanSlot0_, (Hagine::Vector3{0.0f, 3.0f, 0.0f}));  // 1つ目
    GameParameter(Hagine::Vector3, sentanSlot1_, (Hagine::Vector3{0.0f, 2.0f, 0.0f})); // 2つ目
    GameParameter(Hagine::Vector3, sentanScale_, (Hagine::Vector3{0.3f, 0.3f, 0.3f}));  // 共通スケール

    // 自転の軸（Fork基準）。Forkは上方向へ伸びているのでYが長手方向
    GameParameter(Hagine::Vector3, spinAxis_, (Hagine::Vector3{0.0f, 1.0f, 0.0f}));

    // ==== 攻撃の当たり判定（Fork基準・デバッグ調整） ====
    // Fork.obj は Y=-2.20〜3.47 の細長い形で、Y=1.3から上が穂先になっている
    // 柄で殴っても当たらないよう、穂先の範囲(Y=1.3〜3.5)だけを覆う
    GameParameter(Hagine::Vector3, colliderSize_, (Hagine::Vector3{0.47f, 1.1f, 0.2f}));
    GameParameter(Hagine::Vector3, colliderOffset_, (Hagine::Vector3{0.0f, 2.4f, 0.08f}));

    // ==== 状態 ====
    // 攻撃モーションによる構えからのズレ（攻撃コンポーネントが設定する）
    Hagine::Vector3 motionOffset_{0.0f, 0.0f, 0.0f};   // 位置
    Hagine::Vector3 motionRotation_{0.0f, 0.0f, 0.0f}; // 回転
    float motionSpin_ = 0.0f;                          // 自分の軸まわりの自転

    /// 1つの当たり判定に、今の有効/無効と攻撃情報を反映する
    /// baseSize はその判定の元の大きさ（武器やSENTANの実寸）
    void ApplyAttackCollider(BodyColliderComponent *collider, const Hagine::Vector3 &baseSize,
                             AttributeType attribute);

    /// 今の判定の広げ具合（閉じている間は武器の形そのまま）
    float GetHitSizeScale() const { return isAttackColliderEnabled_ ? hit_.sizeScale : 1.0f; }

    // 攻撃の当たり判定の有効/無効（後からくっつくSENTANにも同じ状態を配るため覚えておく）
    bool isAttackColliderEnabled_ = false;

    // 判定を開けている間の中身（どの攻撃が出ているかで変わる）
    HitWindow hit_{};

    // 攻撃者（所有はしない・当てた相手に伝えるだけ）
    Hagine::BaseObject *attacker_ = nullptr;

    // 攻撃の当たり判定コンポーネント（Forkが所有）
    std::unique_ptr<BodyColliderComponent> collider_ = nullptr;

    // くっついているSENTAN
    std::vector<std::unique_ptr<Sentan>> sentans_;
};
