#pragma once
#include "Component/Component.h"
#include "type/Vector3.h"

namespace Hagine
{
	class WorldTransform;
} // namespace Hagine

class EnemyMoveComponent : public Component
{
public:
    /// 必要な物はコンストラクタで受け取る
    explicit EnemyMoveComponent(Hagine::WorldTransform* transform, Hagine::Vector3* target, float* moveSpeed, float* radius)
        : transform_(transform), target_(target), moveSpeed_(moveSpeed), radius_(radius){}

    void Update() override;

private:
    // ==== 注入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform* transform_ = nullptr;
    Hagine::Vector3* target_ = nullptr;
	float* moveSpeed_ = nullptr;
    float* radius_ = nullptr;
};
