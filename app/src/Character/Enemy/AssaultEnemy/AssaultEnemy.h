#pragma once
#include "../BaseEnemy.h"
#include "Component/AssaultAttackComponent.h"

#include <memory>

class AssaultEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;

	// 敵の攻撃コンポーネント
	std::unique_ptr<AssaultAttackComponent> attackComponent_ = nullptr;

	// 弾オブジェクト
	std::unique_ptr<Hagine::BaseObject> bulletObject_ = nullptr;
};

