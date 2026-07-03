#include "TestEnemy.h"
#include <Frame/Frame.h>

using namespace Hagine;

void TestEnemy::Update()
{
	Move();

	BaseObject::Update();
}

void TestEnemy::UniqueInit()
{
	status_.cost = 100;
	status_.hp = 1.0f;
	status_.power = 1.0f;
	status_.movementSpeed = 1.0f;
}

void TestEnemy::Move()
{
	transform_->translation_.x = std::sinf( std::numbers::pi_v<float> / Frame::DeltaTime());
}

void TestEnemy::Attack()
{

}
