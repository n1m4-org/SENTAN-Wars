#include "EnemyManager.h"
#include "BaseEnemy.h"
#include "TestEnemy/TestEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>

using namespace Hagine;

void EnemyManager::Init()
{
	std::unique_ptr<BaseEnemy> enemy = std::make_unique<TestEnemy>();
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