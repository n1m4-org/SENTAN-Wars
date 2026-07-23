#pragma once
#include "3d/camera/projection/ViewProjection.h"
#include "BaseEnemy.h"
#include "Character/Enemy/EnemyParameterManager.h"
#include <memory>
#include <list>
#include <queue>
#include <vector>
#include <random>

class BaseEnemy;
enum class AttributeType;

// ウェーブのコスト・出現傾向を定義する構造体
struct WaveData
{
	int maxCost;           // このウェーブで使える最大コスト
	float normalWeight;    // Normalの出現割合（重み）
	float dashWeight;      // Dashの出現割合
	float tankWeight;      // Tankの出現割合
	float assaultWeight;   // Assaultの出現割合
	float spawnInterval;   // このウェーブでの敵の出現間隔（秒）
	AttributeType attributeType; // このウェーブでの敵の属性タイプ
	bool isBossWave;       // ボスウェーブかどうか
	EnemyType bossType;    // ボスウェーブの際に出現するボスのタイプ
};

/// GPUリソース安全解放用の遅延削除用構造体
struct PendingDeleteEnemy
{
	std::unique_ptr<BaseEnemy> enemy;
	int delayFrames = 3; // GPU描画待ち（3フレーム）
};

class EnemyManager
{
public:
	void Init();
	void Update();
	void Finalize();

	void SetTarget(Hagine::Vector3* target, float* radius)
	{
		target_ = target;
		targetRadius_ = radius;
	}

	void SetTarget(float* radius)
	{
		targetRadius_ = radius;
	}

	void SetTarget(Hagine::Vector3* target)
	{
		target_ = target;
	}

	// 外部からWaveDataを直接受け取って開始するようにする
	void StartWave(const WaveData& waveData);

	/// 現在フィールド上に出現して生存している敵の数を取得
	int GetActiveEnemyCount() const { return static_cast<int>(enemies_.size()); }

	/// これから出現予定のスポーン待機中の敵の数を取得
	int GetRemainingSpawnCount() const { return static_cast<int>(spawnQueue_.size()); }

	/// フィールド上の敵 ＋ スポーン待機中の敵の合計残り数を取得
	int GetTotalRemainingEnemyCount() const { return GetActiveEnemyCount() + GetRemainingSpawnCount(); }

	/// 全ての敵が撃破され、かつスポーン待機中の敵もゼロか判定
	bool IsAllEnemiesDefeated() const { return enemies_.empty() && spawnQueue_.empty(); }

	/// ウェーブプリセットの取得 (ウェーブ番号に応じた緩やかな難易度動的スケーリング)
	WaveData GetWavePreset(size_t index) const;

private:
	void SpawnEnemy(EnemyType type);

private:
	std::list<std::unique_ptr<BaseEnemy>> enemies_;
	std::vector<PendingDeleteEnemy> pendingDeletes_;

	Hagine::Vector3* target_ = nullptr;
	float* targetRadius_ = nullptr;

	std::vector<WaveData> wavePresets_;            // ウェーブのプリセットデータ
	std::queue<EnemyType> spawnQueue_;

	float spawnTimer_ = 0.0f;
	float spawnInterval_ = 3.0f;

	// --- スポーン位置の計算・乱数用 ---
	const float spawnDistance_ = 60.0f; // ターゲットからの出現距離
	std::mt19937 randomEngine_;         // 乱数生成器

	int totalSpawnCount_ = 0; // 生成した敵の通し番号

	EnableDebug("EnemyTestTarget");
	GameParameter(Hagine::Vector3, testTarget_, Hagine::Vector3(5.0f, 0.0f, 5.0f));
	GameParameter(float, testTargetRadius_, 1.5f);
};