#pragma once
#include "3d/Object/Base/BaseObject.h"
#include "Component/Component.h"
#include "Component/ComponentContainer.h"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

/// 本体は機能を持たず、振る舞いは汎用Component派生に分割する
/// 各コンポーネントには「必要な物だけ」を挿入するイメージ
///
/// SENTANの装備で攻撃が増えるため、実行中にもコンポーネントを追加できる
class Player : public Hagine::BaseObject, public ComponentContainer {
  public:
    void Init(const std::string className) override;

    void Update() override;

    void Draw(const Hagine::ViewProjection &viewProjection) override;

    /// 生成済みのコンポーネントを追加して所有する（追加時にInitが呼ばれる）
    /// 更新中に追加されても壊れないよう、実際の登録は次の更新の頭で行う
    Component *AddComponent(std::unique_ptr<Component> component) override;

  private:
    /// コンポーネントを生成して登録する
    /// 生成時の必須依存はそのまま args として派生コンポーネントのコンストラクタへ渡す
    /// 生成したコンポーネント（所有権はPlayerが保持しており、呼び出し側は参照だけ使う）
    template <class T, class... Args>
    T *AddComponent(Args &&...args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T *raw = component.get();
        AddComponent(std::unique_ptr<Component>(std::move(component)));
        return raw;
    }

    /// 追加待ちのコンポーネントを登録する
    void FlushPendingComponents();

  private:
    // 振る舞いコンポーネント群（Playerが所有）
    std::vector<std::unique_ptr<Component>> components_;

    // 追加待ちのコンポーネント（更新中の追加でcomponents_を壊さないためのもの）
    std::vector<std::unique_ptr<Component>> pendingComponents_;
};
