#include "TankEnemy.h"

void TankEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Tank);

	attackComponent_ = std::make_unique<LeapAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);
}

void TankEnemy::UniqueUpdate()
{
	attackComponent_->Update();
}
