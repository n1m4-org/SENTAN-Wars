#include "DashEnemy.h"

void DashEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Dash);

	attackComponent_ = std::make_unique<DashAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);
}

void DashEnemy::UniqueUpdate()
{
	attackComponent_->Update();
}
