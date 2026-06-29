#pragma once
#include <3d/Object/Base/BaseObject.h>

struct EnemyStatus
{
	int cost = 0;
	float hp = 0.0f;
	float power = 0.0f;
	float movementSpeed = 0.0f;
};

class BaseEnemy
	: public Hagine::BaseObject
{
public:
	void Init(const std::string className) override;


protected:
	/// <summary>
	/// 固有初期化処理
	/// </summary>
	virtual void UniqueInit() {};


private:
	EnemyStatus status_;
};

