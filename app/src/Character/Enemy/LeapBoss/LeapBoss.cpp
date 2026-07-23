#include "LeapBoss.h"

bool LeapBoss::IsAttacking() const
{
	return attackComponent_ ? attackComponent_->IsActive() : false;
}

void LeapBoss::UniqueInit()
{
	SetTypeParameter(EnemyType::LeapBoss);

	transform_->scale_ = Hagine::Vector3{ 1.0f, 1.0f, 1.0f } * scale_;

	attackComponent_ = std::make_unique<BossLeapAttackComponent>(transform_.get(), &parameter_.attackRange, target_, targetRadius_);

	auto* col = AddCylinderCollider(GetName() + "_LeapBossCollider");
	col->AddCollisionMask("Player");
	SetResolveCollision(true);
	col->SetRadius(scale_);
	col->SetHeight(scale_ * 2.0f);
	col->SetInward(false);
}

void LeapBoss::UniqueUpdate()
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

void LeapBoss::UniqueUpdateEnd()
{
	transform_->translation_.y = (std::max)(transform_->translation_.y, scale_ - 1.0f); // 地面より下に行かないようにする
}
