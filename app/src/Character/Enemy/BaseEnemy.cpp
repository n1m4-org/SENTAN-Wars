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

	moveComponent_ = std::make_unique<EnemyMoveComponent>(transform_.get(), target_, &parameter_.movementSpeed, targetRadius_);
	moveComponent_->Init();
}

void BaseEnemy::Update()
{
	UniqueUpdate();
	if (target_)
	{
		if ((transform_->translation_ - *target_).Length() <= parameter_.attackRange + *targetRadius_)
		{
			// 攻撃範囲内に入ったら攻撃する
		}
		else
		{
			moveComponent_->Update();
		}
	}

	BaseObject::Update();
}

void BaseEnemy::SetTypeParameter(EnemyType type)
{
	type_ = type;
	parameter_ = EnemyParameterManager::GetInstance()->GetEnemyParameter(type_);
}
