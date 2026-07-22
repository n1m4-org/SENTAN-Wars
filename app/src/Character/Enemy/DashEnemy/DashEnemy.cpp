#include "DashEnemy.h"

void DashEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Dash);

	attackComponent_ = std::make_unique<DashAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);
}

void DashEnemy::UniqueUpdate()
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
