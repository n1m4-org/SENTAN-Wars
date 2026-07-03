#pragma once
#include <3d/Object/Base/BaseObject.h>

struct EnemyStatus
{
	int cost = 0;
	float hp = 0.0f;
	float power = 0.0f;
	float movementSpeed = 0.0f;
	float attackRange = 0.0f;
};

class BaseEnemy
	: public Hagine::BaseObject
{
public:
	void Init(const std::string className) override;

	void Update() override;

	void SetTarget(const Hagine::Vector3& target) { target_ = target; }

protected:
	/// <summary>
	/// 固有初期化処理
	/// </summary>
	virtual void UniqueInit() {};

	/// <summary>
	/// 固有更新処理
	/// </summary>
	virtual void UniqueUpdate() {};

	EnemyStatus status_;

	Hagine::Vector3 target_ = {};
};

