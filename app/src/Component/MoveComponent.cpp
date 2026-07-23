#include "MoveComponent.h"
#include "3d/Camera/projection/ViewProjection.h"
#include "3d/Transform/WorldTransform.h"
#include "Input.h"
#include "myMath.h"
#include "type/Quaternion.h"
#include "utility/vecutl.h"

using namespace Hagine;

void MoveComponent::Update() {
    // 必須依存が無ければ何もしない
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
        transform_->SetRotationQuaternion(
            Quaternion::Slerp(transform_->quateRotation_, CalcFacingRotation(direction), turnLerpRate_));
    }
}

Quaternion MoveComponent::CalcFacingRotation(const Vector3 &direction) const {
    // 進む向きをY軸まわりの角度に直す（移動方向は水平なのでヨーだけで足りる）
    //
    // 符号を反転させているのは、このエンジンの回転の掛かり方に合わせるため
    // QuaternionToMatrix4x4 は列ベクトル形の行列を返すが、変換は v*M の行ベクトルで行うため、
    // クォータニオンは逆回転として効く（そのままだと左右だけ反転して、Aを押すと右を向く）
    const float yaw = utl::vec::VectorToAngle({-direction.x, 0.0f, direction.z});

    return Quaternion::FromEulerAngles({0.0f, yaw, 0.0f});
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
