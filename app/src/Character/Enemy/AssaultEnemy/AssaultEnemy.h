#pragma once
#include "../BaseEnemy.h"
class AssaultEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;
};

