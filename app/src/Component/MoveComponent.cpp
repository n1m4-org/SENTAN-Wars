#include "MoveComponent.h"
#include "3d/Camera/ViewProjection/ViewProjection.h"
#include "3d/Transform/WorldTransform.h"
#include "Input.h"
#include "myMath.h"
#include "type/Quaternion.h"

using namespace Hagine;

void MoveComponent::Update() {
    // 必須依存が無ければ何もしない（防御的チェック）
    if (!transform_) {
        return;
    }

    const Vector3 direction = CalcInputDirection();

    // 入力が無ければ何もしない
    if (direction.LengthSq() <= 0.0f) {
        return;
    }

    // 移動
    transform_->translation_ += direction * moveSpeed_;

    // 進行方向へ体を向ける
    if (faceMoveDirection_) {
        const Quaternion target = Quaternion::FromLookRotation(direction, {0.0f, 1.0f, 0.0f});
        transform_->quateRotation_ = Quaternion::Slerp(transform_->quateRotation_, target, turnLerpRate_);
    }
}

Vector3 MoveComponent::CalcInputDirection() const {
    Input *input = Input::GetInstance();

    // ローカル入力（W:前 S:後 D:右 A:左）
    Vector3 local{0.0f, 0.0f, 0.0f};
    if (input->PushKey(DIK_W)) {
        local.z += 1.0f;
    }
    if (input->PushKey(DIK_S)) {
        local.z -= 1.0f;
    }
    if (input->PushKey(DIK_D)) {
        local.x += 1.0f;
    }
    if (input->PushKey(DIK_A)) {
        local.x -= 1.0f;
    }

    // 斜め移動でも速度を一定にするため正規化
    local = local.Normalize();
    if (local.LengthSq() <= 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }

    // 任意依存(カメラ)が未設定ならワールド軸そのままで移動する
    if (!camera_) {
        return local;
    }

    // カメラ基準：カメラの前方・右方向をXZ平面へ射影して入力を合成する
    Vector3 forward = TransformNormal({0.0f, 0.0f, 1.0f}, camera_->matWorld_);
    Vector3 right = TransformNormal({1.0f, 0.0f, 0.0f}, camera_->matWorld_);
    forward.y = 0.0f;
    right.y = 0.0f;
    forward = forward.Normalize();
    right = right.Normalize();

    return (right * local.x + forward * local.z).Normalize();
}
