#include "EnemyManager.h"
#include "BaseEnemy.h"
#include "TestEnemy/TestEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>
#include "Character/Enemy/EnemyParameterManager.h"

using namespace Hagine;

void EnemyManager::Init()
{
	EnemyParameterManager::GetInstance(); // 敵パラメータの初期化（1度だけ）

	SetTarget(&testTarget_);

	std::unique_ptr<BaseEnemy> enemy = std::make_unique<TestEnemy>();
	enemy->SetTarget(target_);
	enemy->Init("TestEnemy");
	BaseObjectManager::GetInstance()->RegisterExternal(enemy.get());
	enemies_.push_back(std::move(enemy));

}

void EnemyManager::Update()
{

}

void EnemyManager::Finalize()
{
	enemies_.clear();
}