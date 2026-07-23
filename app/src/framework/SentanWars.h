#pragma once

#include <Framework.h>
#include <memory>
#include <debug/DebugEntryManager.h>
#include <debug/DebugMeasure.h>
#include <system/EventListener.h>

class SentanWars : public Hagine::Framework
{
public: // メンバ関数
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;

private:
    DebugEntryManager* pDebugEntryManager_ = nullptr;
    std::unique_ptr<DebugMeasure> pDebugMeasure_ = nullptr;
    EventListener* pEventListener_ = nullptr;
};
