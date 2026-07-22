#pragma once
#include <3d/Object/Base/BaseObject.h>
#include "debug/GameParameter.h"
#include "Component/EnemyMoveComponent.h"
#include "Component/AttributeComponent.h"
#include "Character/Enemy/EnemyParameterManager.h"


class BaseEnemy
	: public Hagine::BaseObject
{
public:
	void Init(const std::string className) override;

	void Update() override;

	void SetTarget(Hagine::Vector3* target, float* radius)
	{
		target_ = target;
		targetRadius_ = radius;
	}

	void SetPos(const Hagine::Vector3& pos)
	{
		transform_->translation_ = pos;
	}

	void SetAttributeType(AttributeType type)
	{
		if (attributeComponent_)
		{
			attributeComponent_->SetType(type);
		}
	}

	const AttributeType GetAttributeType() const
	{
		if (attributeComponent_)
		{
			return attributeComponent_->GetType();
		}
		return AttributeType::Red; // デフォルト値
	}

protected:
	void SetTypeParameter(EnemyType type);

	/// <summary>
	/// 固有初期化処理
	/// </summary>
	virtual void UniqueInit() {};

	/// <summary>
	/// 固有更新処理
	/// </summary>
	virtual void UniqueUpdate() {};

	/// <summary>
	/// 固有更新処理
	/// </summary>
	virtual void UniqueUpdateEnd() {};

	// 敵のステータス
	EnemyParameter parameter_ = {};

	EnemyType type_ = EnemyType::Normal;

	Hagine::Vector3* target_ = nullptr;
	float* targetRadius_ = nullptr;

	// 敵の移動コンポーネント
	std::unique_ptr<EnemyMoveComponent> moveComponent_ = nullptr;

	// 敵の移動コンポーネント
	std::unique_ptr<AttributeComponent> attributeComponent_ = nullptr;

};

