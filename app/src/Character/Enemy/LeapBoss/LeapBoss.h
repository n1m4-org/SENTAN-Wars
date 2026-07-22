#pragma once
#include "../BaseEnemy.h"
#include "Component/BossLeapAttackComponent.h"

class LeapBoss
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;

	void UniqueUpdateEnd() override;

	// 敵の攻撃コンポーネント
	std::unique_ptr<BossLeapAttackComponent> attackComponent_ = nullptr;

	float scale_ = 5.0f;
};
