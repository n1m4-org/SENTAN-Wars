#pragma once
#include "Camera/ViewProjection/ViewProjection.h"
#include "BaseEnemy.h"
#include "Character/Enemy/EnemyParameterManager.h"
#include <memory>
#include <list>

class BaseEnemy;

class EnemyManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void Finalize();

	void SetTarget(Hagine::Vector3* target) { target_ = target; }

private:
	std::list<std::unique_ptr<BaseEnemy>> enemies_;

	Hagine::Vector3* target_ = nullptr;

	EnableDebug("EnemyTestTarget");

	GameParameter(Hagine::Vector3, testTarget_, 0.1f);
};

