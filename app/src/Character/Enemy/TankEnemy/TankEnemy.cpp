#include "TankEnemy.h"

bool TankEnemy::IsAttacking() const
{
	return attackComponent_ ? attackComponent_->IsActive() : false;
}

void TankEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Tank);

	attackComponent_ = std::make_unique<LeapAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);

	auto* col = AddSphereCollider(GetName() + "_TankEnemyCollider");
	col->AddCollisionMask("Player");
}

void TankEnemy::UniqueUpdate()
{
	attackComponent_->Update();
	if (!GetColliders().empty())
	{
		if (attackComponent_->IsActive())
		{
			GetColliders()[0]->SetTag("EnemyBullet");
		}
		else
		{
			GetColliders()[0]->SetTag("Enemy");
		}
	}
}
