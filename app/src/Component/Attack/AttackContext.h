#pragma once
#include "Component/Component.h"
#include <memory>
#include <type_traits>

namespace Hagine {
class WorldTransform;
} // namespace Hagine

class Fork;
class AttackStateComponent;

/// 攻撃コンポーネントを生成するのに必要な依存をまとめたもの
/// 攻撃はSENTANの装備時に生成されるため、生成側は個々の攻撃の型を知らない
/// 「攻撃が必要とする物」をこの1つにまとめて渡す
struct AttackContext {
    Hagine::WorldTransform *ownerTransform = nullptr; // 攻撃する本体（プレイヤー）
    Fork *weapon = nullptr;                           // 振る武器（Fork）
    AttackStateComponent *attackState = nullptr;      // 攻撃中フラグの共有先
};

/// 攻撃コンポーネントの生成関数（SENTANの定義テーブルに載せる）
template <class T>
std::unique_ptr<Component> CreateAttack(const AttackContext &context) {
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    return std::make_unique<T>(context);
}
