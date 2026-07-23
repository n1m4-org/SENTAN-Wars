#include "DashEnemy.h"

bool DashEnemy::IsAttacking() const
{
	return attackComponent_ ? attackComponent_->IsActive() : false;
}

void DashEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Dash);

	attackComponent_ = std::make_unique<DashAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);

	AddSphereCollider(GetName() + "_DashEnemyCollider");
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
