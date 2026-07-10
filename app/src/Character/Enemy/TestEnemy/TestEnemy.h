#pragma once
#include "../BaseEnemy.h"
#include "Component/RamAttackComponent.h"

#include <memory>

class TestEnemy
	: public BaseEnemy
{
public:
	void Update() override;

private:
	void UniqueInit() override;

	// 敵の攻撃コンポーネント
	std::unique_ptr<RamAttackComponent> attackComponent_ = nullptr;
};
