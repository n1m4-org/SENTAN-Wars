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

    /// 属性相性（優勢: 1.5倍, 劣勢: 0.75倍, 同等: 1.0倍）を計算する
    /// Red > Green > Blue > Red
    static float GetAttributeMultiplier(AttributeType attacker, AttributeType defender) {
        if (attacker == defender) return 1.0f;
        if ((attacker == AttributeType::Red && defender == AttributeType::Green) ||
            (attacker == AttributeType::Green && defender == AttributeType::Blue) ||
            (attacker == AttributeType::Blue && defender == AttributeType::Red)) {
            return 1.5f; // 優勢
        }
        if ((attacker == AttributeType::Red && defender == AttributeType::Blue) ||
            (attacker == AttributeType::Green && defender == AttributeType::Red) ||
            (attacker == AttributeType::Blue && defender == AttributeType::Green)) {
            return 0.75f; // 劣勢
        }
        return 1.0f;
    }

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
