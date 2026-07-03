#pragma once
#include "../BaseEnemy.h"
class NormalEnemy
	: public BaseEnemy
{
private:
	void UniqueInit() override;

	void UniqueUpdate() override;
};

