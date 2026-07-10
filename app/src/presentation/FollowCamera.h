#pragma once
#include <memory>
#include <debug/GameParameter.h>
#include <system/ScopedCursorHidden.h>
#include "CameraInput.h"
#include <Camera/ViewProjection/ViewProjection.h>
#include <Transform/WorldTransform.h>
#include <type/Vector3.h>
#include <type/Vector2.h>

namespace Hagine { class WinApp; }

class FollowCamera
{
public:
    FollowCamera(Hagine::ViewProjection& vp) : vp_(vp) {}
    // winApp: Framework が所有するウィンドウ（DI で受け取る）
    void Initialize(Hagine::WinApp* winApp);
    void Update();

    void SetTarget(const Hagine::WorldTransform* pTarget) { pTarget_ = pTarget; }
    void SetTranslate(const Hagine::Vector3& translate) { vp_.translation_ = translate; }

    Hagine::ViewProjection& GetViewProjection() { return vp_; }

private:
    void CameraActivationUpdate(const CameraInput::Command& command);
    void CameraDataUpdate(const CameraInput::Command& command);
    void CursorFixUpdate();
    void PitchClamp(float& pitch) const;


    const Hagine::WorldTransform* pTarget_ = nullptr;

    /// Components
    std::unique_ptr<CameraInput> pCameraInput_ = nullptr;
    std::unique_ptr<ScopedCursorHidden> pCursorHidden_ = nullptr;

    // 所有せず参照のみ（Framework が所有）
    Hagine::WinApp* pWinApp_ = nullptr;

    
    EnableDebug("Follow Camera");
    /// Parameters
    GameParameter(Hagine::Vector3, shiftDirection_, Hagine::Vector3({ 0.0f, 0.08f, -0.997f }));
    GameParameter(Hagine::Vector3, targetPositionOffset_, Hagine::Vector3({ 0.0f, 0.0f, 0.0f }));
    GameParameter(float, kOffset_, 20.0f);                  // カメラとターゲットの距離
    GameParameter(float, kFactorLerp_, 0.1f);               // 線形補間の係数
    GameParameter(Hagine::Vector2, kRotation_, {});         // カメラのX軸回転
    GameParameter(bool, isActive_, false);                  // カメラ制御の有効フラグ
    GameParameter(float, kMinPitch_, 0.0f);                 // カメラの最小ピッチ（下方向の回転制限）
    GameParameter(float, kMaxPitch_, 1.57f);                // カメラの最大ピッチ（上方向の回転制限）
    GameParameterView(Hagine::Vector3, rotation_, {});      // ターゲットの位置（デバッグ表示用）
    GameParameterView(Hagine::Vector3, translation_, {});   // ターゲットの位置（デバッグ表示用）

    Hagine::Vector3 targetPositionPre_ = {};

    // 外から取得する
    Hagine::ViewProjection& vp_;
};

