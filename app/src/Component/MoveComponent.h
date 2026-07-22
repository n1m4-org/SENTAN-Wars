#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"
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

  private:
    /// 入力から移動方向を求める
    Hagine::Vector3 CalcInputDirection() const;

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("Move");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform *transform_ = nullptr;    // 動かす対象
    const Hagine::ViewProjection *camera_ = nullptr; // 移動の基準カメラ

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(float, moveSpeed_, 0.1f);        // 移動速度
    GameParameter(bool, faceMoveDirection_, true); // 進行方向へ体を向けるか
    GameParameter(float, turnLerpRate_, 0.2f);     // 進行方向へ向く回転の補間係数（0で回転しない,1で即時）
};
