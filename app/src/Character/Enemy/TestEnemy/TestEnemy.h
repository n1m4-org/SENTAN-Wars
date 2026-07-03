#pragma once
#include "../BaseEnemy.h"

class TestEnemy
	: public BaseEnemy
{
public:
	void Update() override;

private:
	void UniqueInit() override;

	void Move();

	void Attack();
};

