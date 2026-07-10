#include "CameraInput.h"
#include <WinApp.h>
#include <Input/GamePad.h>

#include <utility/wndutl.h>
#include <winuser.h>

void CameraInput::Update()
{
    POINT pointCenter = utl::window::GetCenterOfWindow(pWinApp_->GetHwnd());
    Hagine::Vector2 center = { static_cast<float>(pointCenter.x), static_cast<float>(pointCenter.y) };

    /// パッド入力 
    Hagine::GamePad* pGamePad = pInput_->GetGamePad();
    Hagine::Vector2 padDelta = { 0.0f, 0.0f };
    if (pGamePad->IsConnected())
    {
        padDelta = { pGamePad->GetRightStickX(), pGamePad->GetRightStickY() };
        if (padDelta.Length() < 0.15f) // スティックのデッドゾーン
        {
            padDelta = { 0.0f, 0.0f };
        }
        padDelta.y*= -1.0f; // スティックの上下は逆になるので反転
        padDelta *= 5.0f; // スティックの値を適当な倍率で調整
    }

    auto cursorPos = pInput_->GetMousePos();
    auto cursorDelta = cursorPos - center;
    Hagine::Vector2 totalDelta = cursorDelta + padDelta;
    command_.delta.yaw = totalDelta.x * sensitivityX;
    command_.delta.pitch = totalDelta.y * sensitivityY;

    command_.isCameraActivationTriggered = 
        pInput_->TriggerKey(DIK_ESCAPE) || pGamePad->IsTrigger(XINPUT_GAMEPAD_RIGHT_THUMB); // 右スティック押込でカメラ操作有効
}
