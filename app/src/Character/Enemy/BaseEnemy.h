#pragma once
#include <3d/Object/Base/BaseObject.h>
#include "debug/GameParameter.h"
#include "Component/EnemyMoveComponent.h"
#include "Component/AttributeComponent.h"
#include "Component/Attack/AttackInfo.h"
#include "Character/Enemy/EnemyParameterManager.h"
#include <Particle/gpu/ParticleCSEmitter.h>


class BaseEnemy
	: public Hagine::BaseObject
{
public:
	virtual ~BaseEnemy();

	void Init(const std::string className) override;

	void Update() override;

	/// 自身の攻撃情報を取得
	AttackInfo GetAttackInfo() const;

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

	/// <summary>
	/// ダメージを受ける処理
	/// </summary>
	/// <param name="damage">基本ダメージ量</param>
	/// <param name="attackerAttribute">攻撃側の属性</param>
	virtual void TakeDamage(float damage, AttributeType attackerAttribute = AttributeType::Red);

	/// <summary>
	/// 攻撃情報構造体を用いたダメージを受ける処理
	/// </summary>
	/// <param name="attackInfo">攻撃情報</param>
	virtual void TakeDamage(const AttackInfo& attackInfo);

	/// 攻撃中かどうかを取得する
	virtual bool IsAttacking() const;

	/// HPゲッター
	float GetHp() const { return currentHp_; }
	float GetMaxHp() const { return maxHp_; }
	bool IsDead() const { return isDead_; }

protected:
	/// 衝突時のコールバック
	virtual void OnHit(Hagine::ColliderBase* other);
	void SetTypeParameter(EnemyType type);

	/// 指定位置にヒットエフェクトを出す（プレイヤーの攻撃を受けたとき）
	void SpawnHitEffect(const Hagine::Vector3& position);

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

	// HP & 被弾パラメータ
	float currentHp_ = 0.0f;
	float maxHp_ = 0.0f;
	bool isDead_ = false;

	// 無敵時間＆演出用
	float invincibilityTimer_ = 0.0f;
	float invincibilityDuration_ = 0.2f; // 無敵時間（秒）
	float hitFlashTimer_ = 0.0f;         // 赤色フラッシュ演出用タイマー
	Hagine::Vector4 originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 攻撃レジストリ登録トラッキング用
	bool isAttackRegistered_ = false;

	// ヒットエフェクトを出す高さ（体の半径の何倍を中心から上へずらすか）
	// 中心に出すと体に埋もれて見えないため、上端あたりへ持ち上げる
	static constexpr float kHitEffectHeightRate_ = 0.8f;
};

