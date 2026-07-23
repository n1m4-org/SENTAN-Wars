#include "AssaultEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>

using namespace Hagine;

AssaultEnemy::~AssaultEnemy()
{
	if (bulletObject_ && !bulletObject_->GetColliders().empty())
	{
		AttackRegistry::Unregister(bulletObject_->GetColliders()[0].get());
	}
}

bool AssaultEnemy::IsAttacking() const
{
	return attackComponent_ ? attackComponent_->IsActive() : false;
}

void AssaultEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Assault);

	bulletObject_ = std::make_unique<BaseObject>();
	bulletObject_->Init(GetName() + "_bulletObject");
	bulletObject_->CreatePrimitiveModel(PrimitiveType::Sphere);
	bulletObject_->GetWorldTransform()->scale_ = Vector3(1.0f, 1.0f, 1.0f) * 0.5f;
	bulletObject_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });

	// 弾用コライダーの生成と登録
	auto* bulletCollider = bulletObject_->AddSphereCollider(GetName() + "_AssaultBulletCollider");
	bulletCollider->SetTag("EnemyBullet");
	bulletCollider->AddCollisionMask("Player");
	AttackRegistry::Register(bulletCollider, GetAttackInfo());

	BaseObjectManager::GetInstance()->RegisterExternal(bulletObject_.get());

	attackComponent_ = std::make_unique<AssaultAttackComponent>(transform_.get(), &parameter_.attackRange, bulletObject_->GetWorldTransform(), target_, targetRadius_);
	attackComponent_->Init(GetName());

	auto* col = AddCylinderCollider(GetName() + "_Collider");
	col->AddCollisionMask("Player");
	SetResolveCollision(true);
}

void AssaultEnemy::UniqueUpdate()
{
	attackComponent_->Update();
}
