#include "BossLeapAttackComponent.h"
#include "Frame/Frame.h"

using namespace Hagine;

void BossLeapAttackComponent::Update()
{
	// 1. 攻撃中 または クールタイム中の処理
	if (attackTimer_ != 0.0f)
	{
		float prevTimer = attackTimer_;
		attackTimer_ += Frame::DeltaTime();

		if (prevTimer < attackTime_)
		{
			float currentClampedTimer = (std::min)(attackTimer_, attackTime_);

			// 指定した時間における想定座標を計算するラムダ
			auto calcPos = [&](float timer) -> Vector3
				{
					float progress = timer / attackTime_;
					float lerpFactor = 0.0f;

					lerpFactor = progress;

					// Y軸の円弧運動を含めた絶対座標を算出
					Vector3 pos = startPos_ + (endPos_ - startPos_) * lerpFactor;
					float baseY = startPos_.y + (endPos_.y - startPos_.y) * lerpFactor;
					float arcY = std::sin(lerpFactor * std::numbers::pi_v<float>) * jumpHeight_;
					pos.y = baseY + arcY;
					return pos;
				};

			Vector3 prevPos = calcPos(prevTimer);
			Vector3 currentPos = calcPos(currentClampedTimer);

			// Lerpではなく、計算された前回フレームからの差分ベクトルを加算
			transform_->translation_ += (currentPos - prevPos);
		}
		else
		{
			isActive_ = false;
		}

		if (attackTimer_ >= coolTime_)
		{
			attackTimer_ = 0.0f;
		}

		return;
	}

	// 2. 待機中の距離判定
	if ((transform_->translation_ - *target_).Length() <= *attackRange_ + *radius_)
	{
		startPos_ = transform_->translation_;
		endPos_ = *target_;
		isActive_ = true;
		attackTimer_ += Frame::DeltaTime();
	}
}
