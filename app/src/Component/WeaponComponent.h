#pragma once
#include "Character/Player/Sentan/Fork.h"
#include "Character/Player/Sentan/SentanDefinition.h"
#include "Component/Component.h"
#include <functional>
#include <memory>
#include <vector>

namespace Hagine {
class BaseObject;
class ViewProjection;
} // namespace Hagine

class ComponentContainer;
class AttackStateComponent;
class JumpComponent;
class MoveComponent;

/// SENTANを装備したときに配られる情報
/// 受け取る側が個々のSENTANの種類を知らなくても扱えるよう、まとめて渡す
struct SentanEquipEvent {
    SentanId id;                                  // 装備したSENTANの種類
    Sentan *sentan = nullptr;                     // Forkにくっついた本体
    const SentanDefinition *definition = nullptr; // その種類の定義
    size_t slotIndex = 0;                         // 何本目のスロットか（0始まり）
    Component *unlockedComponent = nullptr;       // 解禁された振る舞い（無ければnullptr）
};

/// 武器(Fork)を持ち、取得したSENTANを装備する汎用コンポーネント
///
/// SENTANを装備すると、そのSENTANが解禁するコンポーネントを所有者へ追加する
/// 装備していないSENTANの振る舞いはコンポーネントとして存在すらしないため、
/// 「セットしたSENTANの振る舞いだけが呼ばれる」が判定なしで成立する
class WeaponComponent : public Component {
  public:
    /// SENTANを装備したときに呼ばれるコールバック
    using EquipCallback = std::function<void(const SentanEquipEvent &)>;

    /// owner   : 武器の装備先（親）。ここに追従させる
    /// container : 解禁したコンポーネントの追加先
    /// attackState : 攻撃コンポーネント同士で共有する状態
    /// jump : ジャンプの振る舞い（回数を増やすSENTANが使う）
    /// move : 移動の振る舞い（攻撃中に速度を鈍らせるSENTANが使う）
    WeaponComponent(Hagine::BaseObject *owner, ComponentContainer *container, AttackStateComponent *attackState,
                    JumpComponent *jump, MoveComponent *move)
        : owner_(owner), container_(container), attackState_(attackState), jump_(jump), move_(move) {}

    void Init() override;

    void Draw(const Hagine::ViewProjection &viewProjection) override;

    /// 準備フェーズで取得したSENTANを装備する
    /// Forkにくっつけ、そのSENTANの振る舞いを解禁する（装備できなければfalse）
    bool EquipSentan(SentanId id);

    /// 装備中のSENTANを全部外す
    /// SENTANが解禁した振る舞いはコンポーネントごと破棄されるので、外した攻撃は存在しなくなる
    void UnequipAllSentan();

    /// SENTANを装備したときの通知先を追加する
    /// 装備より前に登録しておくこと（過去の装備は遡って通知されない）
    void AddEquipCallback(EquipCallback callback);

    /// 武器本体
    Fork *GetFork() { return fork_.get(); }

  private:
    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::BaseObject *owner_ = nullptr;         // 武器の装備先（親）
    ComponentContainer *container_ = nullptr;     // 解禁した振る舞いの追加先
    AttackStateComponent *attackState_ = nullptr; // 攻撃コンポーネントが共有する状態
    JumpComponent *jump_ = nullptr;               // ジャンプの振る舞い
    MoveComponent *move_ = nullptr;               // 移動の振る舞い

    // 武器本体（このコンポーネントが所有）。Forkは1本だけ
    std::unique_ptr<Fork> fork_ = nullptr;

    // 装備を知りたい相手の通知先（誰が聞いているかはこちらから知らない）
    std::vector<EquipCallback> equipCallbacks_;

    // SENTANが解禁した振る舞い（所有はcontainer_側・外すときに消すために覚えておく）
    std::vector<Component *> unlockedComponents_;
};
