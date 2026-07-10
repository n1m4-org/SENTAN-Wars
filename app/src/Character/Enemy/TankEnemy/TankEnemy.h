#pragma once
#include "../BaseEnemy.h"
#include "Component/LeapAttackComponent.h"

class TankEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;

	// 敵の攻撃コンポーネント
	std::unique_ptr<LeapAttackComponent> attackComponent_ = nullptr;
};

