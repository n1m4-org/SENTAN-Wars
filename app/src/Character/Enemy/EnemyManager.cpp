#include "EnemyManager.h"
#include "BaseEnemy.h"
#include "TestEnemy/TestEnemy.h"
#include "NormalEnemy/NormalEnemy.h"
#include "TankEnemy/TankEnemy.h"
#include "DashEnemy/DashEnemy.h"
#include "AssaultEnemy/AssaultEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>
#include "Character/Enemy/EnemyParameterManager.h"

using namespace Hagine;

void EnemyManager::Init()
{
	EnemyParameterManager::GetInstance(); // 敵パラメータの初期化（1度だけ）

	SetTarget(&testTarget_, &testTargetRadius_);

	std::unique_ptr<BaseEnemy> enemy = std::make_unique<TestEnemy>();
	enemy->SetTarget(target_, targetRadius_);
	enemy->Init("TestEnemy");
	BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
	enemies_.push_back(std::move(enemy));

	enemy = std::make_unique<NormalEnemy>();
	enemy->SetTarget(target_, targetRadius_);
	enemy->Init("NormalEnemy");
	BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
	enemies_.push_back(std::move(enemy));

	enemy = std::make_unique<TankEnemy>();
	enemy->SetTarget(target_, targetRadius_);
	enemy->Init("TankEnemy");
	BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
	enemies_.push_back(std::move(enemy));

	enemy = std::make_unique<DashEnemy>();
	enemy->SetTarget(target_, targetRadius_);
	enemy->Init("DashEnemy");
	BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
	enemies_.push_back(std::move(enemy));

	/*enemy = std::make_unique<AssaultEnemy>();
	enemy->SetTarget(target_, targetRadius_);
	enemy->Init("AssaultEnemy");
	BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
	enemies_.push_back(std::move(enemy));*/
}

void EnemyManager::Update()
{
}

void EnemyManager::Finalize()
{
	enemies_.clear();
}