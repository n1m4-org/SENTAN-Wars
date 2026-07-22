#pragma once
#include "../BaseEnemy.h"
#include "Component/RamAttackComponent.h"

class NormalEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;

	// 敵の攻撃コンポーネント
	std::unique_ptr<RamAttackComponent> attackComponent_ = nullptr;
};

