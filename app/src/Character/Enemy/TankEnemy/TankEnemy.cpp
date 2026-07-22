#include "TankEnemy.h"

void TankEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Tank);

	attackComponent_ = std::make_unique<LeapAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);
}

void TankEnemy::UniqueUpdate()
{
	attackComponent_->Update();
	if (!GetColliders().empty())
	{
		if (attackComponent_->IsActive())
		{
			SetResolveCollision(false);
			GetColliders()[0]->SetTag("EnemyBullet");
		}
		else
		{
			SetResolveCollision(true);
			GetColliders()[0]->SetTag("Enemy");
		}
	}
}
