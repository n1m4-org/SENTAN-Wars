#include "NormalEnemy.h"

void NormalEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Normal);

	attackComponent_ = std::make_unique<RamAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);
}

void NormalEnemy::UniqueUpdate()
{
	attackComponent_->Update();
}
