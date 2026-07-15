#include "DashAttackComponent.h"
#include "Frame/Frame.h"

using namespace Hagine;

void DashAttackComponent::Update()
{
	// 1. 攻撃中 または クールタイム中の処理
	if (attackTimer_ != 0.0f)
	{
		attackTimer_ += Frame::DeltaTime();
		if (attackTimer_ <= attackTime_)
		{
			// 全体の進行度
			float progress = attackTimer_ / attackTime_;

			// イージング
			auto easeOutCubic = [&](float& num)
				{
					num = num < 0.5f ? 4.0f * num * num * num : 1.0f - std::pow(-2.0f * num + 2.0f, 3.0f) / 2.0f;
				};

			easeOutCubic(progress);

			// Lerpに渡す割合
			float lerpFactor = progress;

			// 攻撃処理を実行
			transform_->translation_ = Lerp(startPos_, endPos_, lerpFactor);
		}

		if (attackTimer_ >= coolTime_)
		{
			attackTimer_ = 0.0f; // タイマーをリセット
		}

		return; // 攻撃・クールタイム中はここで処理を終え、以下の距離判定をスキップする
	}

	// 2. 待機中の距離判定
	if (true)
	{
		startPos_ = transform_->translation_;
		endPos_ = startPos_ + ((*target_ - transform_->translation_).Normalize() * (*attackRange_));
		attackTimer_ += Frame::DeltaTime();
	}
}

