#include "FollowCamera.h"

#include <Windows.h>
#include <utility/wndutl.h>
#include <WinApp.h>
#include <algorithm>
#include <type/Matrix4x4.h>
#include <Math/MyMath.h>

void FollowCamera::Initialize()
{
#ifdef _DEBUG
    debug_shiftDirection_.SetOnChange([&](const Hagine::Vector3&)
    {
        shiftDirection_ = shiftDirection_.Normalize();
    });
#else
    isActive_ = true; // デバッグモード以外では常にカメラ制御を有効にする
#endif // _DEBUG

    pCameraInput_ = std::make_unique<CameraInput>();
}

void FollowCamera::Update()
{
#ifdef _DEBUG
    translation_ = vp_.translation_;
    rotation_ = vp_.eulerRotation_;
#endif // _DEBUG

    pCameraInput_->Update();

    const auto& command = pCameraInput_->GetCommand();

    this->CameraActivationUpdate(command);
    this->CursorFixUpdate();

    if (isActive_) this->CameraDataUpdate(pCameraInput_->GetCommand());
    else this->CameraDataUpdate({});
}

void FollowCamera::CameraActivationUpdate(const CameraInput::Command& command)
{
    if (command.isCameraActivationTriggered) isActive_ = !isActive_;

    if (!pCursorHidden_ && isActive_)
    {
        pCursorHidden_ = std::make_unique<ScopedCursorHidden>();
    }
    else if (pCursorHidden_ && !isActive_)
    {
        pCursorHidden_.reset();
    }
}

void FollowCamera::CameraDataUpdate(const CameraInput::Command& command)
{
    if (!pTarget_) return;

    // カメラ制御のトリガーがあるときはカメラの有効/無効を切り替えるだけで、カメラの回転や位置の更新は行わない
    // カーソルを固定するタイミングでカメラの回転や位置の更新も行うと、カーソルが固定された瞬間にカメラが大きく動いてしまうため
    if (command.isCameraActivationTriggered) return;

    // 入力を反映
    kRotation_.x += command.delta.pitch;
    kRotation_.y += command.delta.yaw;

    // ピッチを制限する
    this->PitchClamp(kRotation_.x);

    // 方向を計算
    Hagine::Vector3 rotate = { kRotation_.x, kRotation_.y, 0.0f };
    Hagine::Matrix4x4 rotMatrix = Hagine::MakeRotateXYZMatrix(rotate);
    Hagine::Vector3 direction = Hagine::TransformNormal(shiftDirection_, rotMatrix);

    // 補間
    Hagine::Vector3 targetPosition = pTarget_->translation_ + targetPositionOffset_;
    Hagine::Vector3 lerpedPosition = targetPositionPre_ * (1.0f - kFactorLerp_) + targetPosition * kFactorLerp_;

    Hagine::Vector3 nextCameraPosition = direction.Normalize() * kOffset_ + lerpedPosition;

    vp_.eulerRotation_ = rotate;
    vp_.translation_ = nextCameraPosition;

    targetPositionPre_ = lerpedPosition;
}

void FollowCamera::CursorFixUpdate()
{
    if (!pCursorHidden_ || !isActive_) return;

    auto center = utl::window::GetCenterOfWindow();

    ClientToScreen(Hagine::WinApp::GetInstance()->GetHwnd(), &center);
    SetCursorPos(center.x, center.y);
}

void FollowCamera::PitchClamp(float& pitch) const
{
    pitch = std::clamp(pitch, kMinPitch_, kMaxPitch_);
}