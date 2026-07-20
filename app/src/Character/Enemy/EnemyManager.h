#pragma once
#include "Camera/projection/ViewProjection.h"
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

	void SetTarget(Hagine::Vector3* target, float* radius)
	{ 
		target_ = target;
		targetRadius_ = radius;
	}

private:

	std::list<std::unique_ptr<BaseEnemy>> enemies_;

	Hagine::Vector3* target_ = nullptr;
	float* targetRadius_ = nullptr;

	EnableDebug("EnemyTestTarget");

	GameParameter(Hagine::Vector3, testTarget_, Hagine::Vector3(5.0f, 0.0f, 5.0f));
	GameParameter(float, testTargetRadius_, 1.0f);
};

