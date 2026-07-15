#pragma once
#include "../BaseEnemy.h"
#include "Component/DashAttackComponent.h"

class DashEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;

	// 敵の攻撃コンポーネント
	std::unique_ptr<DashAttackComponent> attackComponent_ = nullptr;
};

