#include "TestEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>

using namespace Hagine;

void TestEnemy::Update()
{
	attackComponent_->Update();
	BaseEnemy::Update();
}

void TestEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Assault);

	bulletObject_ = std::make_unique<BaseObject>();
	bulletObject_->Init(GetName() + "_bulletObject");
	bulletObject_->CreatePrimitiveModel(PrimitiveType::Sphere);
	bulletObject_->GetWorldTransform()->scale_ = Vector3(1.0f, 1.0f, 1.0f) * 0.5f;
	bulletObject_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	BaseObjectManager::GetInstance()->RegisterExternal(bulletObject_.get());

	attackComponent_ = std::make_unique<AssaultAttackComponent>(transform_.get(), &parameter_.attackRange, bulletObject_->GetWorldTransform(), target_, targetRadius_);
	attackComponent_->Init(GetName());
}
