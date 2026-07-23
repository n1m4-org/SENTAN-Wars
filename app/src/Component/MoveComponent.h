#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Quaternion.h"
#include "type/Vector3.h"

namespace Hagine {
class WorldTransform;
class ViewProjection;
} // namespace Hagine

/// 移動させる汎用コンポーネント
class MoveComponent : public Component {
  public:

    /// コンストラクタで受け取る
    explicit MoveComponent(Hagine::WorldTransform *transform) : transform_(transform) {}

    void Update() override;

    /// 未設定ならワールド軸で移動し、設定するとカメラ基準の移動になる
    void SetReferenceCamera(const Hagine::ViewProjection *camera) { camera_ = camera; }

    /// 移動速度の倍率を設定する（1.0で通常、0.0で止まる）
    /// 攻撃中だけ移動を鈍らせたい、といった一時的な調整に使う
    void SetSpeedScale(float scale) { speedScale_ = scale; }

  private:
    /// 入力から移動方向を求める
    Hagine::Vector3 CalcInputDirection() const;

    /// 進む向きへ体を向けるための回転を求める
    Hagine::Quaternion CalcFacingRotation(const Hagine::Vector3 &direction) const;

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("Move");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform *transform_ = nullptr;    // 動かす対象
    const Hagine::ViewProjection *camera_ = nullptr; // 移動の基準カメラ

    // ==== 状態 ====
    float speedScale_ = 1.0f; // 移動速度の倍率（攻撃中などに外から一時的に変えられる）

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(float, moveSpeed_, 0.4f);        // 移動速度
    GameParameter(bool, faceMoveDirection_, true); // 進行方向へ体を向けるか
    GameParameter(float, turnLerpRate_, 0.2f);     // 進行方向へ向く回転の補間係数（0で回転しない,1で即時）
};
