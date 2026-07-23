#include "EnemyManager.h" 
#include "BaseEnemy.h" 
#include "NormalEnemy/NormalEnemy.h" 
#include "TankEnemy/TankEnemy.h" 
#include "DashEnemy/DashEnemy.h" 
#include "AssaultEnemy/AssaultEnemy.h" 
#include "LeapBoss/LeapBoss.h" 
#include <3d/Object/Base/BaseObjectManager.h> 
#include "Character/Enemy/EnemyParameterManager.h" 
#include "Frame/Frame.h"
#include <cmath>
#ifdef _DEBUG
#include <debug/imgui/ImGuizmoManager.h>
#endif

using namespace Hagine;

void EnemyManager::Init()
{
	EnemyParameterManager::GetInstance();
	SetTarget(&testTarget_, &testTargetRadius_);

	// 乱数生成器の初期化（シード値の設定）
	std::random_device rd;
	randomEngine_.seed(rd());

	// ウェーブごとのパラメータ（コストの上限と、各種族のスポーン比率）をプリセット登録
	// 例：
	// Wave 0: 低コスト、Normal主体
	wavePresets_.push_back(WaveData{ 2000, 10.0f, 2.0f, 1.0f, 0.0f, 4.0f, AttributeType::Red, false, EnemyType::LeapBoss });
	// Wave 1: 中コスト、DashやTankも混ざり始める
	wavePresets_.push_back(WaveData{ 3500, 5.0f,  4.0f, 3.0f, 1.0f, 2.5f, AttributeType::Green, false, EnemyType::LeapBoss });
	// Wave 2: 高コスト、高密度
	wavePresets_.push_back(WaveData{ 5000, 3.0f,  3.0f, 5.0f, 5.0f, 1.5f, AttributeType::Blue, false, EnemyType::LeapBoss });
	// Wave 3: ボスウェーブ (指定したボス1体のみ)
	wavePresets_.push_back(WaveData{ 500,  0.0f,  0.0f, 0.0f, 0.0f, 1.5f, AttributeType::Blue, true, EnemyType::LeapBoss });

}

WaveData EnemyManager::GetWavePreset(size_t index) const
{
	// ボスウェーブ（4の倍数でインデックス 3, 7, 11...）の場合
	if (index % 4 == 3)
	{
		return WaveData{ 500, 0.0f, 0.0f, 0.0f, 0.0f, 1.5f, AttributeType::Blue, true, EnemyType::LeapBoss };
	}

	int waveCount = static_cast<int>(index);

	// コスト: 初期800からウェーブごとに+450ずつゆるやかに増加
	int maxCost = 800 + waveCount * 450;

	// スポーン間隔: 初期4.0秒から徐々に短縮（最短1.0秒まで）
	float spawnInterval = (std::max)(1.0f, 4.0f - waveCount * 0.2f);

	// 敵の種族比率（ウェーブが進むとTankやAssaultの比率が徐々にアップ）
	float normalWeight  = (std::max)(2.0f, 10.0f - waveCount * 0.5f);
	float dashWeight    = 2.0f + waveCount * 0.4f;
	float tankWeight    = 1.0f + waveCount * 0.3f;
	float assaultWeight = (waveCount >= 2) ? (1.0f + waveCount * 0.5f) : 0.0f;

	// 属性タイプを巡回（Red -> Green -> Blue -> Red...）
	AttributeType attr = static_cast<AttributeType>(waveCount % 3);

	return WaveData{
		maxCost,
		normalWeight,
		dashWeight,
		tankWeight,
		assaultWeight,
		spawnInterval,
		attr,
		false,
		EnemyType::LeapBoss
	};
}

void EnemyManager::StartWave(const WaveData& waveData)
{
	// キューの初期化
	while (!spawnQueue_.empty()) spawnQueue_.pop();

	if (waveData.isBossWave)
	{
		// ボスウェーブなら指定されたボスの種類を1体のみキューに詰めて終了する
		spawnQueue_.push(waveData.bossType);
		spawnInterval_ = waveData.spawnInterval;
		spawnTimer_ = 0.0f;
		return;
	}

	int remainingCost = waveData.maxCost;

	// 残りコストがなくなるまで、出現予定の敵を事前抽選してキューに詰める
	while (true)
	{
		std::vector<EnemyType> affordableTypes;
		std::vector<float> weights;
		float totalWeight = 0.0f;

		auto checkAndAdd = [&](EnemyType type, float weight)
			{
				int cost = EnemyParameterManager::GetInstance()->GetEnemyParameter(type).cost;
				if (cost <= remainingCost && weight > 0.0f)
				{
					affordableTypes.push_back(type);
					weights.push_back(weight);
					totalWeight += weight;
				}
			};

		// 引数で受け取った waveData を使う
		checkAndAdd(EnemyType::Normal, waveData.normalWeight);
		checkAndAdd(EnemyType::Dash, waveData.dashWeight);
		checkAndAdd(EnemyType::Tank, waveData.tankWeight);
		checkAndAdd(EnemyType::Assault, waveData.assaultWeight);

		if (affordableTypes.empty()) break;

		// メンバ変数の randomEngine_ を使って抽選
		std::uniform_real_distribution<float> dist(0.0f, totalWeight);
		float roll = dist(randomEngine_);
		float currentSum = 0.0f;
		EnemyType chosenType = EnemyType::Normal;

		for (size_t i = 0; i < affordableTypes.size(); ++i)
		{
			currentSum += weights[i];
			if (roll <= currentSum)
			{
				chosenType = affordableTypes[i];
				break;
			}
		}

		spawnQueue_.push(chosenType);
		remainingCost -= EnemyParameterManager::GetInstance()->GetEnemyParameter(chosenType).cost;
	}

	// ウェーブデータからスポーン間隔をセット
	spawnInterval_ = waveData.spawnInterval;

	spawnTimer_ = 0.0f;
}

void EnemyManager::Update()
{
	// 変更なし（前回のコードのまま）
	if (!spawnQueue_.empty())
	{
		float deltaTime = Frame::DeltaTime();
		spawnTimer_ += deltaTime;

		if (spawnTimer_ >= spawnInterval_)
		{
			spawnTimer_ -= spawnInterval_;
			EnemyType nextEnemy = spawnQueue_.front();
			spawnQueue_.pop();
			SpawnEnemy(nextEnemy);
		}
	}

	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}

	// 1. 死亡した敵（HPが0または生存フラグがfalseの敵）を遅延削除キューへ移動
	for (auto it = enemies_.begin(); it != enemies_.end(); )
	{
		if (!(*it) || !(*it)->GetIsAlive() || (*it)->IsDead())
		{
			if (*it)
			{
				(*it)->SetIsModelDraw(false); // 描画を確実に停止
				if (!(*it)->GetColliders().empty())
				{
					(*it)->GetColliders()[0]->SetEnabled(false); // 衝突判定を無効化
				}
				// BaseObjectManagerの描画・更新対象から即座に登録解除
#ifdef _DEBUG
				ImGuizmoManager::GetInstance()->RemoveTarget((*it)->GetName());
#endif
				BaseObjectManager::GetInstance()->UnregisterExternal((*it).get());
			}
			pendingDeletes_.push_back(PendingDeleteEnemy{ std::move(*it), 3 });
			it = enemies_.erase(it);
		}
		else
		{
			++it;
		}
	}

	// 2. 遅延削除キューのカウント減算と、GPU描画完了後の安全解放
	std::erase_if(pendingDeletes_, [](PendingDeleteEnemy& pending) {
		pending.delayFrames--;
		return pending.delayFrames <= 0;
	});
}

void EnemyManager::SpawnEnemy(EnemyType type)
{
	std::unique_ptr<BaseEnemy> enemy = nullptr;
	std::string className = "";

	// 生成のたびにカウントを増やす
	totalSpawnCount_++;

	switch (type)
	{
	case EnemyType::Normal:
		enemy = std::make_unique<NormalEnemy>();
		className = "NormalEnemy_" + std::to_string(totalSpawnCount_);
		break;
	case EnemyType::Tank:
		enemy = std::make_unique<TankEnemy>();
		className = "TankEnemy_" + std::to_string(totalSpawnCount_);
		break;
	case EnemyType::Dash:
		enemy = std::make_unique<DashEnemy>();
		className = "DashEnemy_" + std::to_string(totalSpawnCount_);
		break;
	case EnemyType::Assault:
		enemy = std::make_unique<AssaultEnemy>();
		className = "AssaultEnemy_" + std::to_string(totalSpawnCount_);
		break;
	case EnemyType::LeapBoss:
		enemy = std::make_unique<LeapBoss>();
		className = "LeapBoss_" + std::to_string(totalSpawnCount_);
		break;
	}


	switch (type)
	{
	case EnemyType::Normal:
	case EnemyType::Tank:
	case EnemyType::Dash:
	case EnemyType::Assault:
		//--- 雑魚敵のスポーン ---
		if (enemy)
		{
			enemy->SetTarget(target_, targetRadius_);
			enemy->Init(className);

			// --- ターゲットから一定距離・ランダムな角度（円周上）に配置 ---
			if (target_)
			{
				// 0 ～ 2π (360度) の範囲でランダムな角度を生成
				std::uniform_real_distribution<float> angleDist(0.0f, std::numbers::pi_v<float> *2.0f);
				float angle = angleDist(randomEngine_);

				// ターゲットの位置を基準に、サイン・コサインを使ってX, Zのオフセットを計算
				Hagine::Vector3 spawnPos = *target_;
				spawnPos.x += std::cos(angle) * spawnDistance_;
				spawnPos.z += std::sin(angle) * spawnDistance_;
				spawnPos.y = 0.0f; // 地面など、高さを固定したい場合はここで指定

				// 計算した座標をセット (BaseEnemy.h にある SetPos を使用)
				enemy->SetPos(spawnPos);
			}

			BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
			enemies_.push_back(std::move(enemy));
		}

		break;


	case EnemyType::LeapBoss:
		if (enemy)
		{
			enemy->SetTarget(target_, targetRadius_);
			enemy->Init(className);

			if (target_)
			{
				// 正面方向とするベクトル（軸のプラス方向）
				Hagine::Vector3 forwardDir = Hagine::Vector3(0.0f, 0.0f, 1.0f);

				// ターゲットの位置から、正面方向に spawnDistance_ 分だけ離した位置を計算
				Hagine::Vector3 spawnPos = *target_ + (forwardDir * spawnDistance_);
				spawnPos.y = 0.0f; // 地面の高さに固定

				enemy->SetPos(spawnPos);
			}

			BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
			enemies_.push_back(std::move(enemy));
		}
		break;
	}
}

void EnemyManager::Finalize()
{
	for (auto& enemy : enemies_)
	{
		if (enemy)
		{
			enemy->SetIsModelDraw(false);
			if (!enemy->GetColliders().empty())
			{
				enemy->GetColliders()[0]->SetEnabled(false);
			}
#ifdef _DEBUG
			ImGuizmoManager::GetInstance()->RemoveTarget(enemy->GetName());
#endif
			BaseObjectManager::GetInstance()->UnregisterExternal(enemy.get());
		}
	}
	enemies_.clear();
	pendingDeletes_.clear();
}