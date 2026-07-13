#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"

/// 属性の種類（赤・青・緑）
/// プレイヤーのSENTAN(武器)と敵の両方で共有する
enum class AttributeType {
    Red = 0,   // 赤
    Blue = 1,  // 青
    Green = 2, // 緑
};

/// 属性を1つ保持するコンポーネント
/// SENTAN(武器)と敵に付けられる
class AttributeComponent : public Component {
  public:
    AttributeComponent() = default;
    explicit AttributeComponent(AttributeType type) { SetType(type); }

    /// 属性を設定する
    void SetType(AttributeType type) { typeIndex_ = static_cast<int>(type); }

    /// 属性を取得する
    AttributeType GetType() const { return ToAttributeType(typeIndex_); }

  private:
    /// int -> AttributeType 変換
    static AttributeType ToAttributeType(int index) {
        switch (index) {
        case static_cast<int>(AttributeType::Blue):
            return AttributeType::Blue;
        case static_cast<int>(AttributeType::Green):
            return AttributeType::Green;
        default:
            return AttributeType::Red;
        }
    }

  private:
    // GameParameterの登録先
    EnableDebug("Attribute");

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(int, typeIndex_, 0);
};
