#include "EnemyMoveComponent.h"
#include "3d/Transform/WorldTransform.h"
#include "myMath.h"
#include "Frame/Frame.h"

using namespace Hagine;

void EnemyMoveComponent::Update()
{
    // 必須依存が無ければ何もしない（防御的チェック）
    if (!transform_ || !target_ || !moveSpeed_)
    {
        return;
    }

    Vector3 direction = Vector3(*target_ - transform_->translation_).Normalize();

	Vector3 velocity = direction * (*moveSpeed_);

    // 移動
    transform_->translation_ += velocity * Frame::DeltaTime();
}
