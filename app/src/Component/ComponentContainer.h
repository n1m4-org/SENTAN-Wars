#pragma once
#include <memory>

class Component;

/// コンポーネントを後から追加できる入れ物のインターフェース
/// 装備などで「実行中に振る舞いが増える」ものは、本体の型を知らなくてもこれ経由で追加できる
class ComponentContainer {
  public:
    virtual ~ComponentContainer() = default;

    /// 生成済みのコンポーネントを追加して所有する（追加時にInitが呼ばれる）
    /// 所有権は入れ物側が持ち、呼び出し側は戻り値を参照としてだけ使う
    virtual Component *AddComponent(std::unique_ptr<Component> component) = 0;
};
