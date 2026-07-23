#pragma once
#include "3d/Object/Base/BaseObject.h"
#include "Character/Player/Sentan/Sentan.h"
#include "Character/Player/Sentan/SentanDefinition.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"
#include <memory>
#include <vector>

/// プレイヤーの武器本体
/// Forkは常に1本だけで準備フェーズで取得したSENTAN(最大2つ)はこのForkにくっつく
/// 装着したSENTANの位置はForkが決めるので、SENTAN側は位置を持たない
class Fork : public Hagine::BaseObject {
  public:
    void Init(const std::string className) override;

    void Update() override;

    void Draw(const Hagine::ViewProjection &viewProjection) override;

    /// SENTANをくっつける（最大2つまで）
    Sentan *AttachSentan(SentanId id);

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

    // SENTANは最大2つまで
    static constexpr size_t kMaxSentanCount = 2;

    // ==== 状態 ====
    // 攻撃モーションによる構えからのズレ（攻撃コンポーネントが設定する）
    Hagine::Vector3 motionOffset_{0.0f, 0.0f, 0.0f};   // 位置
    Hagine::Vector3 motionRotation_{0.0f, 0.0f, 0.0f}; // 回転
    float motionSpin_ = 0.0f;                          // 自分の軸まわりの自転

    // くっついているSENTAN
    std::vector<std::unique_ptr<Sentan>> sentans_;
};
