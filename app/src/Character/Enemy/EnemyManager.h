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

// ウェーブのコスト・出現傾向を定義する構造体
struct WaveData
{
	int maxCost;           // このウェーブで使える最大コスト
	float normalWeight;    // Normalの出現割合（重み）
	float dashWeight;      // Dashの出現割合
	float tankWeight;      // Tankの出現割合
	float assaultWeight;   // Assaultの出現割合
	float spawnInterval;   // このウェーブでの敵の出現間隔（秒）
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

	// 外部からWaveDataを直接受け取って開始するようにする
	void StartWave(const WaveData& waveData);

private:
	void SpawnEnemy(EnemyType type);

private:
	std::list<std::unique_ptr<BaseEnemy>> enemies_;

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
	GameParameter(float, testTargetRadius_, 1.0f);
};