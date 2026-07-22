#include "RamAttackComponent.h"
#include "Frame/Frame.h"

using namespace Hagine;

void RamAttackComponent::Update()
{
	// 1. 攻撃中 または クールタイム中の処理
	if (attackTimer_ != 0.0f)
	{
		float prevTimer = attackTimer_;
		attackTimer_ += Frame::DeltaTime();

		if (prevTimer < attackTime_)
		{
			float currentClampedTimer = (std::min)(attackTimer_, attackTime_);

			// 任意の時間における往復運動の移動割合を計算するラムダ
			auto calcFactor = [](float timer, float maxTime)
				{
					float progress = timer / maxTime;
					progress = progress < 0.5f ? 4.0f * progress * progress * progress : 1.0f - std::pow(-2.0f * progress + 2.0f, 3.0f) / 2.0f;

					if (progress <= 0.5f)
					{
						return progress * 2.0f; // 前半：ターゲットへ向かう
					}
					else
					{
						return (1.0f - progress) * 2.0f; // 後半：元の場所へ戻る
					}
				};

			float prevFactor = calcFactor(prevTimer, attackTime_);
			float currentFactor = calcFactor(currentClampedTimer, attackTime_);

			// 前フレームからの差分（速度ベクトル）を座標に加算
			transform_->translation_ += (endPos_ - startPos_) * (currentFactor - prevFactor);
		}
		else
		{
			isActive_ = false;
		}

		if (attackTimer_ >= coolTime_)
		{
			attackTimer_ = 0.0f; // タイマーをリセット
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
