#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"

namespace Hagine {
class WorldTransform;
class ViewProjection;
} // namespace Hagine

/// WASDキーで対象のTransformをXZ平面上で移動させる汎用コンポーネント
/// 動かす対象(Transform)を外部から注入するため、特定のクラスに依存しない
/// 例)Player・2人目のPlayer・敵など、Transformを持つものなら誰でも使い回せる
class MoveComponent : public Component {
  public:

    /// 【必須依存】をコンストラクタで注入する
    /// 動かす対象のTransformは「手足」のように無いと成立しない＝最初から必要なので
    /// コンストラクタで受け取る
    explicit MoveComponent(Hagine::WorldTransform *transform) : transform_(transform) {}

    void Update() override;

    /// 【任意依存】をセッターで差し込む
    /// 移動の基準にするカメラは「服」のように後から着替えできる／無くても成立する
    /// 未設定ならワールド軸で移動し、設定するとカメラ基準の移動になる
    void SetReferenceCamera(const Hagine::ViewProjection *camera) { camera_ = camera; }

  private:
    /// 入力から移動方向を求める（XZ平面・正規化済み。入力が無ければゼロベクトル）
    Hagine::Vector3 CalcInputDirection() const;

  private:
    // GameParameterの登録先となるデバッグエントリ
    EnableDebug("Move");

    // ==== 注入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform *transform_ = nullptr;    // 必須：動かす対象
    const Hagine::ViewProjection *camera_ = nullptr; // 任意：移動の基準カメラ

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(float, moveSpeed_, 0.1f);        // 移動速度（1フレームあたりの移動量）
    GameParameter(bool, faceMoveDirection_, true); // 進行方向へ体を向けるか
    GameParameter(float, turnLerpRate_, 0.2f);     // 進行方向へ向く回転の補間係数（0で回転しない,1で即時）
};
