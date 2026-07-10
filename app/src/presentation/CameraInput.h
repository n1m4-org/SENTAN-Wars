#pragma once
#include <Input.h>
#include <type/Vector2.h>
#include <debug/GameParameter.h>

namespace Hagine { class WinApp; }

class CameraInput
{
public:
    struct Command
    {
        struct Delta
        {
            float pitch; // カメラの上下回転
            float yaw;   // カメラの左右回転
        } delta;
        bool isCameraActivationTriggered = false; // カメラ操作がトリガーされたかどうか
    };

    // ウィンドウは所有せず、Framework が所有するものを注入で受け取る
    explicit CameraInput(Hagine::WinApp* winApp) : pWinApp_(winApp) {}

    void Update();
    Command GetCommand() const { return command_; }

private:
    EnableDebug("Camera Input");
    GameParameter(float, sensitivityX, 0.01f);
    GameParameter(float, sensitivityY, 0.01f);

    Command command_ = {};
    Hagine::Input* pInput_ = Hagine::Input::GetInstance();
    Hagine::WinApp* pWinApp_ = nullptr;
};