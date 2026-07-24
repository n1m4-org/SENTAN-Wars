#pragma once
#include "BaseScene.h"
#include <array>
#include <memory>
#include <Sprite.h>
#include <presentation/ui/ResultFrame.h>
#include <presentation/ui/ClearTimeView.h>
#include <presentation/ui/RankingView.h>
#include <logic/ranking/RankingBoard.h>
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

	/// <summary>
	/// ランキングビューを生成・初期化する
	/// </summary>
	void InitializeRanking();

	/// <summary>
	/// ランキングへの突き落とし挿入演出を開始する（新記録のときのみ）
	/// </summary>
	void StartRankingInsert();

	/// <summary>
	/// 演出（枠展開 → クリアタイム → ランキング）を進める。
	/// シーン遷移が空けきってから呼ぶ。
	/// </summary>
	/// <param name="deltaTime">経過時間</param>
	void UpdateDirection(float deltaTime);

	/// <summary>
    /// カメラを更新
    /// </summary>
	void UpdateCamera();

	/// <summary>
   /// シーン遷移を実行
   /// </summary>
	void ChangeScene();

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

	// ランキング
	static constexpr size_t kRankRowCount_ = RankingBoard::kEntryCount;   // 表示する順位数（RankingBoard::kEntryCount に追従）
	static constexpr float kRankFontSize_ = 48.0f;                        // 順位行のタイムの高さ
	static constexpr float kRankNumberGap_ = 20.0f;                       // 順位番号とタイムの間隔
	static constexpr float kRankTopGap_ = 52.0f;                          // クリアタイムと順位表の間隔
	static constexpr float kRankRowSpacing_ = 66.0f;                      // 順位行どうしの間隔

	RankingBoard ranking_;
	RankingView rankingView_;
	int myRank_ = -1;   // 今回のタイムが入った順位。-1 は該当なし（ランク外／デモ）
	bool insertStarted_ = false;   // 挿入演出を開始済みか
	std::array<float, kRankRowCount_> oldRankingEntries_ = {};   // 挿入前の上位 kRankRowCount_ 件
	std::array<float, kRankRowCount_> newRankingEntries_ = {};   // 挿入後の上位 kRankRowCount_ 件

	// レイアウト（Initialize で算出）
	Hagine::Vector2 frameCenter_ = {};      // 枠の中心
	Hagine::Vector2 frameSize_ = {};        // 枠のサイズ
	float leftX_ = 0.0f;                    // 左揃えの基準 X
	float labelCenterY_ = 0.0f;             // ラベルの縦中心
	float timeCenterY_ = 0.0f;              // クリアタイムの縦中心
	std::array<float, kRankRowCount_> rankRowCenterY_ = {};   // 各順位行の縦中心
	Hagine::Vector2 labelFullSize_ = {};    // ラベルの最終サイズ
	static constexpr float kLabelHeight_ = 64.0f;   // ラベルの最終高さ
	float timeFullFontSize_ = 104.0f;       // クリアタイムの最終フォントサイズ（大きめ）
};
