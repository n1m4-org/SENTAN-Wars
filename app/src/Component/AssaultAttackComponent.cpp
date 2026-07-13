#include "AssaultAttackComponent.h"
#include "Frame/Frame.h"
#include <3d/Object/Base/BaseObjectManager.h>

using namespace Hagine;

void AssaultAttackComponent::Init()
{
	AoEObject_ = std::make_unique<BaseObject>();
	AoEObject_->Init("AoEObject");
	AoEObject_->CreatePrimitiveModel(PrimitiveType::Sphere);
	AoEObject_->GetWorldTransform()->scale_ = { 1.0f, 0.0f, 1.0f };
	AoEObject_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	BaseObjectManager::GetInstance()->RegisterExternal(AoEObject_.get());
}

void AssaultAttackComponent::Update()
{
	// 1. 攻撃中 または クールタイム中の処理
	if (attackTimer_ != 0.0f)
	{
		attackTimer_ += Frame::DeltaTime();
		if (attackTimer_ <= attackTime_)
		{
			// 全体の進行度
			float progress = attackTimer_ / attackTime_;

			// Lerpに渡す割合
			float lerpFactor = progress;

			// 攻撃処理を実行
			attackTransform_->translation_ = Lerp(startPos_, endPos_, lerpFactor);
			float baseY = std::lerp(startPos_.y, endPos_.y, lerpFactor);
			float height = 3.0f; // アークの高さを調整
			float arcY = height * 4.0f * progress * (1.0f - progress);
			attackTransform_->translation_.y = baseY + arcY;

		}
		else
		{
			AoEObject_->SetIsModelDraw(false);
			attackTransform_->translation_ = transform_->translation_;
		}

		if (attackTimer_ >= coolTime_)
		{
			attackTransform_->translation_ = startPos_;
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
		AoEObject_->SetIsModelDraw(true);
		AoEObject_->GetWorldTransform()->translation_ = endPos_;
		AoEObject_->GetWorldTransform()->translation_.y -= *radius_;
	}
}
