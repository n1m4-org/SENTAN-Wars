#pragma once
#include "3d/Object/Base/BaseObject.h"
#include "Component/Component.h"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

/// 本体は機能を持たず、振る舞いは汎用Component派生に分割する（コンポーネント指向）
/// 各コンポーネントには「必要な物だけ」を注入する
class Player : public Hagine::BaseObject {
  public:
    void Init(const std::string className) override;

    void Update() override;

    void Draw(const Hagine::ViewProjection &viewProjection) override;

  private:
    /// コンポーネントを生成して登録する
    /// 生成時の必須依存はそのまま args として派生コンポーネントのコンストラクタへ渡す
    /// 生成したコンポーネント（所有権はPlayerが保持。呼び出し側は参照だけ使う）
    template <class T, class... Args>
    T *AddComponent(Args &&...args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T *raw = component.get();
        components_.emplace_back(std::move(component));
        return raw;
    }

  private:
    // 振る舞いコンポーネント群（Playerが所有）
    std::vector<std::unique_ptr<Component>> components_;
};
