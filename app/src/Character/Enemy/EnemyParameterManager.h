#pragma once
#include "debug/GameParameter.h"

// =========================================================
// 敵パラメータ一括登録用マクロ
// (EnableDebugの変数名被りを防ぐため、prefixを使って一意な変数名にする)
// =========================================================
#define REGISTER_ENEMY_PARAMETERS(prefix, categoryName, init_cost, init_hp, init_power, init_speed, init_range) \
    DebugEntry debugEntry_##prefix{ std::source_location::current().file_name(), categoryName }; \
    GameParameter(int,   prefix##_cost,          init_cost); \
    GameParameter(float, prefix##_hp,            init_hp); \
    GameParameter(float, prefix##_power,         init_power); \
    GameParameter(float, prefix##_movementSpeed, init_speed); \
    GameParameter(float, prefix##_attackRange,   init_range)

enum class EnemyType
{
	Normal,
	Dash,
	Tank,
	Assault,
	LeapBoss
};

struct EnemyParameter
{
    int cost = 100;
    float hp = 10.0f;
    float power = 1.0f;
    float movementSpeed = 1.0f;
    float attackRange = 1.0f;
};

// 敵の「種族ごとの基本設定」だけを管理するクラス
class EnemyParameterManager
{
public:
    static EnemyParameterManager* GetInstance()
    {
        static EnemyParameterManager instance;
        return &instance;
    }

	// 戻り値は値で返す
	// ここで作るEnemyParameterはその場限りのものなので、参照で返すと
	// 呼び出し元が消えた領域を読むことになる（種族ごとの値が壊れる）
	EnemyParameter GetEnemyParameter(EnemyType type) const
	{
		switch (type)
		{
		case EnemyType::Normal:
			return { normal_cost, normal_hp, normal_power, normal_movementSpeed, normal_attackRange };
		case EnemyType::Dash:
			return { dash_cost, dash_hp, dash_power, dash_movementSpeed, dash_attackRange };
		case EnemyType::Tank:
			return { tank_cost, tank_hp, tank_power, tank_movementSpeed, tank_attackRange };
		case EnemyType::Assault:
			return { assault_cost, assault_hp, assault_power, assault_movementSpeed, assault_attackRange };
		case EnemyType::LeapBoss:
			return { leapBoss_cost, leapBoss_hp, leapBoss_power, leapBoss_movementSpeed, leapBoss_attackRange };
		default:
			return {};
		}
	}

private:
	EnemyParameterManager() = default;
	~EnemyParameterManager() = default;
	// 各種族の初期パラメータ（プログラム起動時に1度だけImGuiに登録される）
	// prefix, categoryName, cost, hp, power, speed, range
	REGISTER_ENEMY_PARAMETERS(normal,   "Enemy: Normal",   100, 20.0f,  1.0f, 2.5f,  1.0f);
	REGISTER_ENEMY_PARAMETERS(dash,     "Enemy: Dash",     120, 10.0f,  1.0f, 0.01f, 22.0f);
	REGISTER_ENEMY_PARAMETERS(tank,     "Enemy: Tank",     200, 50.0f,  1.0f, 1.5f,  1.0f);
	REGISTER_ENEMY_PARAMETERS(assault,  "Enemy: Assault",  100, 10.0f,  1.0f,  2.5f,  25.0f);
	REGISTER_ENEMY_PARAMETERS(leapBoss, "Enemy: LeapBoss", 500, 150.0f, 10.0f, 0.01f, 100.0f);
};
