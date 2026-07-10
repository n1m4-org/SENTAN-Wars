#pragma once

namespace Hagine {
class ViewProjection;
} // namespace Hagine

/// 振る舞いを分割するコンポーネントの基底クラス
/// コンポーネントが必要とする物（Transform など）は、派生クラスが自分のコンストラクタ／セッターで受け取る
class Component {
  public:
    virtual ~Component() = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    virtual void Init() {}

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update() {}

    /// <summary>
    /// 描画処理
    /// </summary>
    virtual void Draw(const Hagine::ViewProjection &viewProjection) {}
};
