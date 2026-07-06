#include "TestEnemy.h"
#include <Frame/Frame.h>
#include "Character/Enemy/EnemyParameterManager.h"

using namespace Hagine;

void TestEnemy::Update()
{
	BaseEnemy::Update();
}

void TestEnemy::UniqueInit()
{
	SetTypeParameter(EnemyType::Normal);
}
