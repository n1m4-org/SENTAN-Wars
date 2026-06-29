#include "BaseEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>

using namespace Hagine;

void BaseEnemy::Init(const std::string className)
{
	BaseObject::Init(className);
	// 仮モデル
	CreatePrimitiveModel(PrimitiveType::Sphere);

	UniqueInit();
}
