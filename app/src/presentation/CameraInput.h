#pragma once
#include <Input.h>
#include <type/Vector2.h>
#include <debug/GameParameter.h>

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

    void Update();
    Command GetCommand() const { return command_; }

private:
    EnableDebug("Camera Input");
    GameParameter(float, sensitivityX, 0.01f);
    GameParameter(float, sensitivityY, 0.01f);

    Command command_ = {};
    Hagine::Input* pInput_ = Hagine::Input::GetInstance();
};