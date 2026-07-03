#pragma once

/// 振る舞いを分割する汎用コンポーネントの基底クラス
/// このクラスは所有者（Player等）を一切知らない
/// Player が Component を、Component が Player を相互に指す「循環参照」を避ける
/// コンポーネントが必要とする物（Transform など）は、派生クラスが
/// 自分のコンストラクタ／セッターで受け取る（差し込む）
class Component {
  public:
    virtual ~Component() = default;

    /// <summary>]
    ///初期化（登録後に一度だけ呼ばれる）
    /// </summary>
    virtual void Init() {}

    /// <summary>
    ///更新（毎フレーム呼ばれる）
    /// </summary>
    virtual void Update() {}
};
