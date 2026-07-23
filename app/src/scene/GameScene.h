#pragma once
#include "BaseScene.h"
#include <logic/wave/WaveDirector.h>

// application
#include <Character/Player/Player.h>
#include <presentation/FollowCamera.h>
#include <Character/Enemy/EnemyManager.h>
#include <presentation/ui/HpHudView.h>
#include <presentation/HudManager.h>
#include <presentation/ui/WaveCountHudView.h>
#include <presentation/ui/RemainingEnemyCountHudView.h>

/// <summary>
/// テストシーンのクラス
/// シーンの設定などを確認するシーン
/// </summary>
class GameScene : public Hagine::BaseScene
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

    /// <summary>
    /// クリアタイムの計測を停止し、結果を SceneManager に記録する。
    /// クリア条件が未確定のため、任意のタイミングで呼び出せる（2回目以降は無視）。
    /// </summary>
    void StopClearTime();

    /// <summary>
    /// 計測中の経過時間（秒）を取得する
    /// </summary>
    /// <returns>float: 経過時間（秒）</returns>
    float GetClearTime() const { return clearTimer_; }

private:
    /// ===================================================
    /// private method
    /// ===================================================

    /// <summary>
    /// フォローカメラを初期化
    /// </summary>
    /// <param name="pTarget"></param>
    void InitializeFollowCamera(const Hagine::WorldTransform* pTarget);

    /// <summary>
    /// カメラを更新
    /// </summary>
    void UpdateCamera();

    /// <summary>
    /// シーン遷移を実行
    /// </summary>
    void ChangeScene();

    /// <summary>
    /// HUDマネージャを初期化
    /// </summary>
    void InitializeHudManager();

    /// <summary>
    /// HUDを更新
    /// </summary>
    void UpdateHud();

private:
    /// ===================================================
    /// private variants
    /// ===================================================

    // playerの宣言
	std::unique_ptr<Player> player_ = nullptr;
    std::unique_ptr<WaveDirector> pWaveDirector_ = nullptr;
    std::unique_ptr<FollowCamera> pFollowCamera_ = nullptr;

    // HUD関連の宣言
    std::unique_ptr<HudManager> pHudManager_ = nullptr;
	std::unique_ptr<EnemyManager> pEnemyManager_ = nullptr;
	HpHudView* pHpHudView_ = nullptr;
    WaveCountHudView* pWaveCountHudView_ = nullptr;
    RemainingEnemyCountHudView* pRemainEnemyCountHudView_ = nullptr;

    // クリアタイム計測
    float clearTimer_ = 0.0f;      // 計測中の経過時間（秒）
    bool isTimerRunning_ = false;  // 計測中フラグ
};
