#include "BaseEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>

using namespace Hagine;

void BaseEnemy::Init(const std::string className)
{
	BaseObject::Init(className);
	// 仮モデル
	CreatePrimitiveModel(PrimitiveType::Sphere);

	SetTexture("debug/white1x1.png");

	UniqueInit();

	GetColliders()[0]->AddCollisionMask("Player");
	GetColliders()[0]->AddCollisionMask("Enemy");

	parameter_.attackRange += *targetRadius_; // 攻撃範囲にターゲットの半径を加算しておく

	moveComponent_ = std::make_unique<EnemyMoveComponent>(transform_.get(), target_, &parameter_.movementSpeed, targetRadius_);
	moveComponent_->Init();

}

void BaseEnemy::Update()
{
	UniqueUpdate();
	if (target_)
	{
		if ((transform_->translation_ - *target_).Length() > parameter_.attackRange + *targetRadius_)
		{
			moveComponent_->Update();
		}
	}

	transform_->translation_.y = (std::max)(transform_->translation_.y, 0.0f); // 地面より下に行かないようにする

	UniqueUpdateEnd();

	BaseObject::Update();
}

void BaseEnemy::SetTypeParameter(EnemyType type)
{
	type_ = type;
	parameter_ = EnemyParameterManager::GetInstance()->GetEnemyParameter(type_);
}
