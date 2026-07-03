#include "TankEnemy.h"

void TankEnemy::UniqueInit()
{
	status_.cost = 100;
	status_.hp = 1.0f;
	status_.power = 1.0f;
	status_.movementSpeed = 1.0f;
	status_.attackRange = 1.0f;
}

void TankEnemy::UniqueUpdate()
{

}
