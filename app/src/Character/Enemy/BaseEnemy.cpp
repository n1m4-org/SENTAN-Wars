#include "BaseEnemy.h"
#include <3d/Object/Base/BaseObjectManager.h>
#include "Frame/Frame.h"
#ifdef _DEBUG
#include <debug/imgui/ImGuizmoManager.h>
#endif

using namespace Hagine;

BaseEnemy::~BaseEnemy()
{
#ifdef _DEBUG
	ImGuizmoManager::GetInstance()->RemoveTarget(GetName());
#endif
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

bool BaseEnemy::IsAttacking() const
{
	auto* nonConstThis = const_cast<BaseEnemy*>(this);
	if (!nonConstThis->GetColliders().empty() && nonConstThis->GetColliders()[0]->GetTag() == "EnemyBullet")
	{
		return true;
	}
	return false;
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

	// 衝突判定タグの事前登録
	auto* tagMgr = Hagine::ColliderTagManager::GetInstance();
	tagMgr->AddTag("Player");
	tagMgr->AddTag("Enemy");
	tagMgr->AddTag("EnemyBullet");
	tagMgr->AddTag("PlayerAttack");
	tagMgr->AddTag("PlayerBullet");

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

	// 攻撃中でない場合、プレイヤーの方向に向く
	if (target_ && !isDead_ && !IsAttacking())
	{
		Vector3 toTarget = *target_ - transform_->translation_;
		toTarget.y = 0.0f; // 水平（XZ平面）方向のみ
		if (toTarget.LengthSq() > 0.0001f)
		{
			// ターゲット方向のYaw角（Y軸周りの回転角）を計算
			float targetYaw = std::atan2(toTarget.x, toTarget.z);

			// 現在のオイラー角回転を取得
			Vector3 currentEuler = transform_->eulerRotation_;

			// 最短角度差の補間 (角度ラッピング考慮)
			float diff = targetYaw - currentEuler.y;
			while (diff < -std::numbers::pi_v<float>) diff += std::numbers::pi_v<float> * 2.0f;
			while (diff > std::numbers::pi_v<float>) diff -= std::numbers::pi_v<float> * 2.0f;

			float turnSpeed = 10.0f;
			float newYaw = currentEuler.y + diff * (std::min)(turnSpeed * deltaTime, 1.0f);

			// eulerRotation_ と quateRotation_ の両方を同期して設定
			transform_->SetRotationEuler({ 0.0f, newYaw, 0.0f });
		}
	}

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
		SetIsModelDraw(false); // 描画を無効化
		if (!GetColliders().empty())
		{
			GetColliders()[0]->SetEnabled(false); // 衝突判定を無効化
		}
	}
}

void BaseEnemy::TakeDamage(const AttackInfo& attackInfo)
{
	TakeDamage(attackInfo.damage, attackInfo.attribute);
}

void BaseEnemy::OnHit(ColliderBase* other)
{
	if (!other || isDead_) return;

	const std::string& tag = other->GetTag();
	// プレイヤーの攻撃タグに対してのみダメージ処理を行う
	if (tag == "PlayerAttack" || tag == "PlayerBullet") // プレイヤーの攻撃タグを入れる
	{
		// AttackRegistry に登録された攻撃情報を検索
		if (const AttackInfo* info = AttackRegistry::Get(other))
		{
			TakeDamage(*info);
			return;
		}
	}
}

void BaseEnemy::SetTypeParameter(EnemyType type)
{
	type_ = type;
	parameter_ = EnemyParameterManager::GetInstance()->GetEnemyParameter(type_);
}
