#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"

namespace Hagine {
	class WorldTransform;
} // namespace Hagine

// スペースキーで対象のTransformをジャンプ（上下移動）させるコンポーネント
class JumpComponent : public Component {
public:

	/// コンストラクタで受け取る
	explicit JumpComponent(Hagine::WorldTransform* transform) : transform_(transform) {}

	void Update() override;

	/// ジャンプ開始（接地しているときだけ上向きの初速を与える）
	void StartJump();

private:
	// GameParameterの登録先となるデバッグ
	EnableDebug("Jump");

	// ==== 挿入された依存（所有はしない・参照するだけ） ====
	Hagine::WorldTransform* transform_ = nullptr; // 必須：動かす対象

	// ==== 状態 ====
	float velocityY_ = 0.0f; // 現在の上下方向の速度
	bool isGrounded_ = true; // 接地しているか

	// ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
	GameParameter(float, jumpStrength_, 0.3f); // ジャンプの初速
	GameParameter(float, gravity_, 0.015f);    // 重力（1フレームあたりの加速）
	GameParameter(float, groundY_, 0.0f);      // 地面の高さ（着地位置）
};
