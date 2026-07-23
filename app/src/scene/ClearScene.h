#pragma once
#include "BaseScene.h"
#include <memory>
#include <Sprite.h>
#include <presentation/ui/ResultFrame.h>
#include <presentation/ui/ClearTimeView.h>
#include "debug/GameParameter.h"

/// <summary>
/// クリアシーンのクラス
/// 枠の展開演出、クリアタイムのカウントアップ演出を行う。
/// （ランキング表示は今後クリアタイムの下に追加予定）
/// </summary>
class ClearScene : public Hagine::BaseScene
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
    void Draw() override {};

    /// <summary>
    /// オフスクリーン描画処理
    /// </summary>
    void DrawForOffScreen() override {};

    /// <summary>
    /// シーン設定を追加
    /// </summary>
    void AddSceneSetting() override;

    /// <summary>
    /// オブジェクト設定を追加
    /// </summary>
    void AddObjectSetting() override {};

    /// <summary>
    /// パーティクル設定を追加
    /// </summary>
    void AddParticleSetting() override {};

    /// <summary>
    /// クリアタイム（秒）を設定する
    /// </summary>
    /// <param name="seconds">クリアタイム（秒）</param>
    void SetClearTime(float seconds) { targetClearSeconds_ = seconds; }

private:
    /// ===================================================
    /// private method
    /// ===================================================

    /// <summary>
    /// 演出の状態
    /// </summary>
    enum class Phase { FrameExpand, Reveal, Done };

    /// <summary>
    /// レイアウトを計算する
    /// </summary>
    void InitializeLayout();

    /// <summary>
    /// スプライトを生成・登録する
    /// </summary>
    void InitializeSprites();

    /// <summary>
    /// 演出を即座に完了させる（SPACE スキップ用）
    /// </summary>
    void Skip();

    /// <summary>
    /// 出現スケール（0.0〜1.0）をラベルとクリアタイムへ反映する
    /// </summary>
    /// <param name="scale">出現スケール</param>
    void ApplyReveal(float scale);

    /// <summary>
    /// カウントアップ中の秒数を反映する
    /// </summary>
    /// <param name="seconds">表示する秒数</param>
    void ApplyCountUp(float seconds);

private:
    /// ===================================================
    /// private variants
    /// ===================================================

    // デバッグエントリ（GameParameter より先に構築する必要があるため先頭で宣言する）
    EnableDebug("ClearScene");

    // 演出時間
    static constexpr float kRevealDuration_ = 0.4f;  // 出現スケール演出にかける時間
    static constexpr float kCountDuration_ = 1.5f;   // カウントアップにかける時間

    Phase phase_ = Phase::FrameExpand;
    float revealTimer_ = 0.0f;
    float countTimer_ = 0.0f;

    // クリアタイム（秒）※将来的にゲーム側から設定される想定
    GameParameter(float, targetClearSeconds_, 87.0f);

    ResultFrame frame_;
    ClearTimeView clearTimeView_;
    std::unique_ptr<Hagine::Sprite> pClearTimeLabel_ = nullptr; // 「CLEAR TIME」の文字

    // レイアウト（Initialize で算出）
    Hagine::Vector2 frameCenter_ = {};      // 枠の中心
    Hagine::Vector2 frameSize_ = {};        // 枠のサイズ
    float leftX_ = 0.0f;                    // 左揃えの基準 X
    float labelCenterY_ = 0.0f;             // ラベルの縦中心
    float timeCenterY_ = 0.0f;              // クリアタイムの縦中心
    Hagine::Vector2 labelFullSize_ = {};    // ラベルの最終サイズ
    static constexpr float kLabelHeight_ = 60.0f;   // ラベルの最終高さ
    float timeFullFontSize_ = 76.0f;        // クリアタイムの最終フォントサイズ
};
