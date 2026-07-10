#include "LeapAttackComponent.h"
#include "Frame/Frame.h"

using namespace Hagine;

void LeapAttackComponent::Update()
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
			auto easeInOutCubic = [&](float& num)
				{
					num = num < 0.5f ? 4.0f * num * num * num : 1.0f - std::pow(-2.0f * num + 2.0f, 3) / 2.0f;
				};


			// Lerpに渡す割合
			float lerpFactor = 0.0f;

			if (progress <= 0.33f)
			{
				// 前半：ターゲットへ向かう
				lerpFactor = progress * 3.0f;
				easeInOutCubic(lerpFactor);
			}
			else if (progress <= 0.66f)
			{
				// 停止
				lerpFactor = 1.0f;
			}
			else
			{
				// 後半：元の場所へ戻る
				lerpFactor = (1.0f - progress) * 3.0f;
			}

			// 攻撃処理を実行
			transform_->translation_ = Lerp(startPos_, endPos_, lerpFactor);
			float baseY = std::lerp(startPos_.y, endPos_.y, lerpFactor);
			float arcY = std::sin(lerpFactor * std::numbers::pi_v<float>) * 3.0f;
			transform_->translation_.y = baseY + arcY;
		}

		if (attackTimer_ >= coolTime_)
		{
			transform_->translation_ = startPos_;
			attackTimer_ = 0.0f; // タイマーをリセット
		}

		return; // 攻撃・クールタイム中はここで処理を終え、以下の距離判定をスキップする
	}

	// 2. 待機中の距離判定
	if ((transform_->translation_ - *target_).Length() <= *attackRange_ + *radius_)
	{
		startPos_ = transform_->translation_;
		endPos_ = *target_;
		attackTimer_ += Frame::DeltaTime();
	}
}

