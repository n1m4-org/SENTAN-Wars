#pragma once
#include <memory>
#include <Sprite.h>
#include <Easing.h>
#include <type/Vector2.h>
#include <type/Vector4.h>

/// <summary>
/// クリアシーンの結果表示枠。
/// white1x1 を用いて、横方向 → 縦方向 の順にイージングで展開し長方形を形成する。
/// 内側は半透明の黒、外周は灰色の縁取りで描画する。
/// </summary>
class ResultFrame
{
public:
    /// <summary>
    /// 枠の設定
    /// </summary>
    struct Config
    {
        Hagine::Vector2 center = {};                                    // 枠の中心座標
        Hagine::Vector2 size = {};                                      // 展開後の枠サイズ
        float borderThickness = 4.0f;                                   // 縁取りの太さ
        float lineThickness = 6.0f;                                     // 横展開時の線の太さ
        float horizontalDuration = 0.75f;                              // 横展開にかける時間
        float verticalDuration = 0.75f;                                // 縦展開にかける時間
        Hagine::Vector4 borderColor = { 0.55f, 0.55f, 0.55f, 1.0f };    // 縁取り色（灰）
        Hagine::Vector4 innerColor = { 0.0f, 0.0f, 0.0f, 0.55f };       // 内側色（半透明の黒）
        Hagine::EasingType horizontalEasing = Hagine::EasingType::OutCubic;
        Hagine::EasingType verticalEasing = Hagine::EasingType::OutCubic;
    };

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="config">枠の設定</param>
    void Initialize(const Config& config);

    /// <summary>
    /// 終了処理（スプライトの登録解除）
    /// </summary>
    void Finalize();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">経過時間</param>
    void Update(float deltaTime);

    /// <summary>
    /// 展開演出を即座に完了させる
    /// </summary>
    void Skip();

    /// <summary>
    /// 展開演出が完了しているか
    /// </summary>
    /// <returns>bool: 完了していれば true</returns>
    bool IsFinished() const { return phase_ == Phase::Finished; }

private:
    /// <summary>
    /// 展開フェーズ
    /// </summary>
    enum class Phase { Horizontal, Vertical, Finished };

    /// <summary>
    /// 現在の枠サイズをスプライトへ反映する
    /// </summary>
    /// <param name="currentSize">現在の枠サイズ</param>
    void ApplyToSprites(const Hagine::Vector2& currentSize);

    Config config_ = {};
    Phase phase_ = Phase::Horizontal;
    float timer_ = 0.0f;

    std::unique_ptr<Hagine::Sprite> pBorder_ = nullptr; // 縁取り（灰色の矩形）
    std::unique_ptr<Hagine::Sprite> pInner_ = nullptr;  // 内側（半透明の黒）
};
