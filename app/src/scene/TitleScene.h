#pragma once
#include "BaseScene.h"
#include <array>
#include <memory>
#include <Sprite.h>

/// <summary>
/// テストシーンのクラス
/// シーンの設定などを確認するシーン
/// </summary>
class TitleScene : public Hagine::BaseScene
{
public:
    /// ===================================================
    /// public method
    /// ===================================================

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;

    /// <summary>
    /// オフスクリーン描画処理
    /// </summary>
    void DrawForOffScreen() override;

    /// <summary>
    /// シーン設定を追加
    /// </summary>
    void AddSceneSetting() override;

    /// <summary>
    /// オブジェクト設定を追加
    /// </summary>
    void AddObjectSetting() override;

    /// <summary>
    /// パーティクル設定を追加
    /// </summary>
    void AddParticleSetting() override;

private:
    enum class SpriteName { Logo, Prompt, kSize };
    std::unique_ptr<Hagine::Sprite>& GetSprite(SpriteName name) { return sprites_[static_cast<size_t>(name)]; }
    void SpriteInitialize();

    std::array<std::unique_ptr<Hagine::Sprite>, static_cast<size_t>(SpriteName::kSize)> sprites_;
};