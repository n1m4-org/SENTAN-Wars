#pragma once
#include "../BaseEnemy.h"
#include "Component/AssaultAttackComponent.h"

#include <memory>

class TestEnemy
	: public BaseEnemy
{
public:
	void Update() override;

private:
	void UniqueInit() override;

	// 敵の攻撃コンポーネント
	std::unique_ptr<AssaultAttackComponent> attackComponent_ = nullptr;

	//
	std::unique_ptr<Hagine::BaseObject> bulletObject_ = nullptr;
};
