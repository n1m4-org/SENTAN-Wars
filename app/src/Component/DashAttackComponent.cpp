#include "DashAttackComponent.h"
#include "Frame/Frame.h"

using namespace Hagine;

void DashAttackComponent::Update()
{
	// 1. 攻撃中 または クールタイム中の処理
	if (attackTimer_ != 0.0f)
	{
		float prevTimer = attackTimer_;
		attackTimer_ += Frame::DeltaTime();

		// 攻撃時間内の場合のみ移動処理を行う[cite: 1]
		if (prevTimer < attackTime_)
		{
			// フレームまたぎでattackTime_を超過した場合の補正
			float currentClampedTimer = (std::min)(attackTimer_, attackTime_);

			// 任意の時間におけるイージングの進行度を計算するラムダ
			auto calcFactor = [](float timer, float maxTime)
				{
					float progress = timer / maxTime;
					return progress < 0.5f ? 4.0f * progress * progress * progress : 1.0f - std::pow(-2.0f * progress + 2.0f, 3.0f) / 2.0f;
				};

			float prevFactor = calcFactor(prevTimer, attackTime_);
			float currentFactor = calcFactor(currentClampedTimer, attackTime_);

			// 前フレームからの進捗分の差分ベクトルを計算し、現在の座標に加算する
			transform_->translation_ += (endPos_ - startPos_) * (currentFactor - prevFactor);
		}

		if (attackTimer_ >= coolTime_)
		{
			attackTimer_ = 0.0f; // タイマーをリセット[cite: 1]
		}

		return; // 攻撃・クールタイム中はここで処理を終え、以下の距離判定をスキップする[cite: 1]
	}
	else
	{
		isActive_ = false;
	}

	// 2. 待機中の距離判定
	if (true)
	{
		startPos_ = transform_->translation_;
		endPos_ = startPos_ + ((*target_ - transform_->translation_).Normalize() * (*attackRange_));
		isActive_ = true;
		attackTimer_ += Frame::DeltaTime();
	}
}
