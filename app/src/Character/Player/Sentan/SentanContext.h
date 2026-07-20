#pragma once
#include "Component/Component.h"
#include <memory>
#include <type_traits>

namespace Hagine {
class WorldTransform;
} // namespace Hagine

class Fork;
class AttackStateComponent;
class JumpComponent;

/// SENTANが解禁するコンポーネントを生成するのに必要な依存をまとめたもの
/// 生成側は個々のコンポーネントの型を知らないため、
/// 「解禁されうる振る舞いが必要とする物」をこの1つにまとめて渡す
struct SentanContext {
    Hagine::WorldTransform *ownerTransform = nullptr; // 振る舞いの持ち主（プレイヤー）
    Fork *weapon = nullptr;                           // 振る武器（Fork）
    AttackStateComponent *attackState = nullptr;      // 攻撃中フラグの共有先
    JumpComponent *jump = nullptr;                    // ジャンプの振る舞い（回数を増やす対象）
};

/// SENTANが解禁するコンポーネントの生成関数（SENTANの定義テーブルに載せる）
template <class T>
std::unique_ptr<Component> CreateSentanComponent(const SentanContext &context) {
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    return std::make_unique<T>(context);
}
