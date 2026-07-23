#include "BaseEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>
#include "Frame/Frame.h"

using namespace Hagine;

BaseEnemy::~BaseEnemy()
{
	if (isAttackRegistered_ && !GetColliders().empty())
	{
		AttackRegistry::Unregister(GetColliders()[0].get());
		isAttackRegistered_ = false;
	}
}

AttackInfo BaseEnemy::GetAttackInfo() const
{
	return AttackInfo{ parameter_.power, GetAttributeType(), const_cast<BaseEnemy*>(this) };
}

void BaseEnemy::Init(const std::string className)
{
	BaseObject::Init(className);
	// 仮モデル
	CreatePrimitiveModel(PrimitiveType::Sphere);

	SetTexture("debug/white1x1.png");

	UniqueInit();

	// HPパラメータの初期化
	maxHp_ = parameter_.hp;
	currentHp_ = maxHp_;
	isDead_ = false;

	if (!GetColliders().empty())
	{
		GetColliders()[0]->AddCollisionMask("Player");
		GetColliders()[0]->AddCollisionMask("Enemy");
		GetColliders()[0]->AddCollisionMask("PlayerAttack");
		GetColliders()[0]->AddCollisionMask("PlayerBullet");

		GetColliders()[0]->SetOnCollisionEnter([this](ColliderBase* other) {
			OnHit(other);
		});
	}

	parameter_.attackRange += *targetRadius_; // 攻撃範囲にターゲットの半径を加算しておく

	moveComponent_ = std::make_unique<EnemyMoveComponent>(transform_.get(), target_, &parameter_.movementSpeed, targetRadius_);
	moveComponent_->Init();
}

void BaseEnemy::Update()
{
	float deltaTime = Frame::DeltaTime();

	// 無敵タイマー減算
	if (invincibilityTimer_ > 0.0f)
	{
		invincibilityTimer_ -= deltaTime;
	}

	// 被弾フラッシュタイマー減算と色復元
	if (hitFlashTimer_ > 0.0f)
	{
		hitFlashTimer_ -= deltaTime;
		if (hitFlashTimer_ <= 0.0f)
		{
			SetColor(originalColor_);
		}
	}

	UniqueUpdate();
	if (target_ && !isDead_)
	{
		if ((transform_->translation_ - *target_).Length() > parameter_.attackRange + *targetRadius_)
		{
			moveComponent_->Update();
		}
	}

	transform_->translation_.y = (std::max)(transform_->translation_.y, 0.0f); // 地面より下に行かないようにする

	UniqueUpdateEnd();

	// 攻撃中（EnemyBulletタグ設定時）の AttackRegistry 自動同期処理
	if (!GetColliders().empty())
	{
		if (GetColliders()[0]->GetTag() == "EnemyBullet")
		{
			if (!isAttackRegistered_)
			{
				AttackRegistry::Register(GetColliders()[0].get(), GetAttackInfo());
				isAttackRegistered_ = true;
			}
		}
		else
		{
			if (isAttackRegistered_)
			{
				AttackRegistry::Unregister(GetColliders()[0].get());
				isAttackRegistered_ = false;
			}
		}
	}

	BaseObject::Update();
}

void BaseEnemy::TakeDamage(float damage, AttributeType attackerAttribute)
{
	if (isDead_ || invincibilityTimer_ > 0.0f)
	{
		return;
	}

	// 属性相性倍率の適用
	float multiplier = AttributeComponent::GetAttributeMultiplier(attackerAttribute, GetAttributeType());
	float finalDamage = damage * multiplier;

	currentHp_ -= finalDamage;

	// 無敵時間とヒットフラッシュの設定
	invincibilityTimer_ = invincibilityDuration_;
	hitFlashTimer_ = 0.1f;
	SetColor({ 1.0f, 0.2f, 0.2f, 1.0f }); // 被弾時は一時的に赤く点滅

	// 死亡判定
	if (currentHp_ <= 0.0f)
	{
		currentHp_ = 0.0f;
		isDead_ = true;
		SetIsAlive(false); // BaseObjectの生存フラグをfalseにする
	}
}

void BaseEnemy::TakeDamage(const AttackInfo& attackInfo)
{
	TakeDamage(attackInfo.damage, attackInfo.attribute);
}

void BaseEnemy::OnHit(ColliderBase* other)
{
	if (!other || isDead_) return;

	// AttackRegistry に登録された攻撃情報を検索
	if (const AttackInfo* info = AttackRegistry::Get(other))
	{
		TakeDamage(*info);
		return;
	}

	// タグ判定による従来のフォールバック処理
	const std::string& tag = other->GetTag();
	if (tag == "PlayerAttack" || tag == "PlayerBullet")
	{
		TakeDamage(10.0f, AttributeType::Red);
	}
}

void BaseEnemy::SetTypeParameter(EnemyType type)
{
	type_ = type;
	parameter_ = EnemyParameterManager::GetInstance()->GetEnemyParameter(type_);
}
