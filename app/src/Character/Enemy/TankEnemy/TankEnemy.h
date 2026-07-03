#pragma once
#include "../BaseEnemy.h"
class TankEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;
};

