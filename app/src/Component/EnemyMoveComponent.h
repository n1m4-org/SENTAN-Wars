#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"

namespace Hagine
{
	class WorldTransform;
	class ViewProjection;
} // namespace Hagine

class EnemyMoveComponent : public Component
{
public:
    /// 必要な物はコンストラクタで受け取る
    explicit EnemyMoveComponent(Hagine::WorldTransform* transform, Hagine::Vector3* target, float* moveSpeed) 
        : transform_(transform), target_(target), moveSpeed_(moveSpeed) {}

    void Update() override;

private:
    // ==== 注入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform* transform_ = nullptr;
    Hagine::Vector3* target_ = nullptr;
	float* moveSpeed_ = nullptr;
};
