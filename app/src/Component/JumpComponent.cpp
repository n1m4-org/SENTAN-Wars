#include "JumpComponent.h"
#include "3d/Transform/WorldTransform.h"
#include "Input.h"

using namespace Hagine;

void JumpComponent::Update() {
	// 必須依存が無ければ何もしない
	if (!transform_) {
		return;
	}

	// スペースキーでジャンプ開始
	Input* input = Input::GetInstance();
	if (input->TriggerKey(DIK_SPACE)) {
		StartJump();
	}

	// 空中にいる間は重力で落下させる
	if (!isGrounded_) {
		velocityY_ -= gravity_;
		transform_->translation_.y += velocityY_;

		// 地面に着いたら止めて接地状態へ
		if (transform_->translation_.y <= groundY_) {
			transform_->translation_.y = groundY_;
			velocityY_ = 0.0f;
			isGrounded_ = true;
		}
	}
}

void JumpComponent::StartJump() {
	// 接地しているときだけジャンプできる（多段ジャンプはしない）
	if (!isGrounded_) {
		return;
	}
	velocityY_ = jumpStrength_;
	isGrounded_ = false;
}
