#pragma once
#include "Component/AttributeComponent.h"
#include <unordered_map>

namespace Hagine {
class BaseObject;
class ColliderBase;
}

/// <summary>
/// 攻撃ヒット時に渡されるパラメータ構造体
/// </summary>
struct AttackInfo {
    float damage = 10.0f;                         // 攻撃力
    AttributeType attribute = AttributeType::Red; // 攻撃属性
    Hagine::BaseObject* owner = nullptr;          // 攻撃者（プレイヤー等）
};

/// <summary>
/// 攻撃オブジェクトや武器、攻撃コンポーネントが実装する情報提供インターフェース
/// </summary>
class IAttackProvider {
public:
    virtual ~IAttackProvider() = default;

    /// <summary>
    /// 攻撃パラメータを取得する
    /// </summary>
    virtual AttackInfo GetAttackInfo() const = 0;
};

/// <summary>
/// コライダーと攻撃情報を紐付けるレジストリクラス
/// エンジン層の ColliderBase を変更せずに攻撃情報を紐付け可能にする
/// </summary>
class AttackRegistry {
public:
    static void Register(const Hagine::ColliderBase* collider, const AttackInfo& info) {
        if (collider) {
            GetRegistry()[collider] = info;
        }
    }

    static void Unregister(const Hagine::ColliderBase* collider) {
        if (collider) {
            GetRegistry().erase(collider);
        }
    }

    static const AttackInfo* Get(const Hagine::ColliderBase* collider) {
        if (!collider) return nullptr;
        auto& reg = GetRegistry();
        auto it = reg.find(collider);
        return (it != reg.end()) ? &it->second : nullptr;
    }

    static void Clear() {
        GetRegistry().clear();
    }

private:
    static std::unordered_map<const Hagine::ColliderBase*, AttackInfo>& GetRegistry() {
        static std::unordered_map<const Hagine::ColliderBase*, AttackInfo> registry;
        return registry;
    }
};
