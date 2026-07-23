#include "HealthComponent.h"
#include <utility>

void HealthComponent::Init() {
    currentHp_ = maxHp_;
    isDead_ = false;
    invincibilityTimer_ = 0.0f;
}

void HealthComponent::Update() {
    if (invincibilityTimer_ > 0.0f) {
        invincibilityTimer_ -= 1.0f;
    }
}

void HealthComponent::TakeDamage(const AttackInfo &attackInfo) {
    TakeDamage(attackInfo.damage, attackInfo.attribute);
}

void HealthComponent::TakeDamage(float damage, AttributeType attackerAttribute) {
    // 死んだ後と、被弾直後の無敵中は受け付けない
    if (isDead_ || invincibilityTimer_ > 0.0f) {
        return;
    }

    // 属性を持っていないものは相性なしの等倍で受ける
    const float multiplier =
        attribute_ ? AttributeComponent::GetAttributeMultiplier(attackerAttribute, attribute_->GetType()) : 1.0f;

    currentHp_ -= damage * multiplier;
    invincibilityTimer_ = invincibilityFrame_;

    if (currentHp_ > 0.0f) {
        return;
    }

    // 尽きたことを知りたい相手へ配る（何が起きるかはこのコンポーネントの担当ではない）
    currentHp_ = 0.0f;
    isDead_ = true;
    for (const DeathCallback &callback : deathCallbacks_) {
        callback();
    }
}

void HealthComponent::AddDeathCallback(DeathCallback callback) {
    if (callback) {
        deathCallbacks_.emplace_back(std::move(callback));
    }
}
