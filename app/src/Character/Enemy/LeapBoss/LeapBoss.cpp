#include "LeapBoss.h"

void LeapBoss::UniqueInit()
{
	SetTypeParameter(EnemyType::LeapBoss);

	transform_->scale_ = Hagine::Vector3{ 1.0f, 1.0f, 1.0f } * scale_;

	attackComponent_ = std::make_unique<BossLeapAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);

	AddCylinderCollider("LeapBossCollider");
	SetResolveCollision(true);
}

void LeapBoss::UniqueUpdate()
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

void LeapBoss::UniqueUpdateEnd()
{
	transform_->translation_.y = (std::max)(transform_->translation_.y, scale_ - 1.0f); // 地面より下に行かないようにする
}
