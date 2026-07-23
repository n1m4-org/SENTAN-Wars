#include "NormalEnemy.h"

bool NormalEnemy::IsAttacking() const
{
	return attackComponent_ ? attackComponent_->IsActive() : false;
}

void NormalEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Normal);

	attackComponent_ = std::make_unique<RamAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);

	auto* col = AddSphereCollider(GetName() + "_NormalEnemyCollider");
	col->AddCollisionMask("Player");
	SetResolveCollision(true);
}

void NormalEnemy::UniqueUpdate()
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
