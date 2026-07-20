#pragma once
#include "Character/Player/Sentan/Fork.h"
#include "Character/Player/Sentan/SentanDefinition.h"
#include "Component/Component.h"
#include <memory>

namespace Hagine {
class BaseObject;
class ViewProjection;
} // namespace Hagine

class ComponentContainer;
class AttackStateComponent;

/// 武器(Fork)を持ち、取得したSENTANを装備する汎用コンポーネント
///
/// SENTANを装備すると、そのSENTANが解禁する攻撃コンポーネントを所有者へ追加する
/// 装備していないSENTANの攻撃はコンポーネントとして存在すらしないため、
/// 「セットしたSENTANの攻撃だけが呼ばれる」が判定なしで成立する
class WeaponComponent : public Component {
  public:
    /// owner   : 武器の装備先（親）。ここに追従させる
    /// container : 解禁した攻撃コンポーネントの追加先
    /// attackState : 攻撃コンポーネント同士で共有する状態
    WeaponComponent(Hagine::BaseObject *owner, ComponentContainer *container, AttackStateComponent *attackState)
        : owner_(owner), container_(container), attackState_(attackState) {}

    void Init() override;

    void Draw(const Hagine::ViewProjection &viewProjection) override;

    /// 準備フェーズで取得したSENTANを装備する
    /// Forkにくっつけ、そのSENTANの攻撃を解禁する（装備できなければfalse）
    bool EquipSentan(SentanId id);

    /// 武器本体
    Fork *GetFork() { return fork_.get(); }

  private:
    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::BaseObject *owner_ = nullptr;         // 武器の装備先（親）
    ComponentContainer *container_ = nullptr;     // 解禁した攻撃の追加先
    AttackStateComponent *attackState_ = nullptr; // 攻撃コンポーネントが共有する状態

    // 武器本体（このコンポーネントが所有）。Forkは1本だけ
    std::unique_ptr<Fork> fork_ = nullptr;
};
