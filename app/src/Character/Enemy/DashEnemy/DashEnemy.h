#pragma once
#include "../BaseEnemy.h"
class DashEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;
};

