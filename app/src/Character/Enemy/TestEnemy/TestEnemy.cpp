#include "TestEnemy.h"
#include <Frame/Frame.h>
#include "Character/Enemy/EnemyParameterManager.h"

using namespace Hagine;

void TestEnemy::Update()
{
	attackComponent_->Update();
	BaseEnemy::Update();
}

void TestEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Normal);

	attackComponent_ = std::make_unique<RamAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);
}
