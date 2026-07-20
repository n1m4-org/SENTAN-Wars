#pragma once
#include "3d/Object/Base/BaseObject.h"
#include "Character/Player/Sentan/SentanDefinition.h"
#include "Component/AttributeComponent.h"
#include <memory>

/// Forkにくっつくパーツ（準備フェーズで取得したSENTAN）
/// SENTANは Type(属性) を持つ。Level・Atk は未実装
///
/// 位置・スケールは装着先のForkが決めるため、ここでは持たない
class Sentan : public Hagine::BaseObject {
  public:
    /// どのSENTANかは定義で決まる（モデルと、解禁する攻撃を持っている）
    explicit Sentan(const SentanDefinition &definition) : definition_(&definition) {}

    void Init(const std::string className) override;

    void Update() override;

    /// このSENTANの定義（モデル・解禁する攻撃）
    const SentanDefinition &GetDefinition() const { return *definition_; }

    /// 属性(Type)を設定する
    void SetType(AttributeType type) {
        if (attribute_) {
            attribute_->SetType(type);
        }
    }

    /// 属性(Type)を取得する
    AttributeType GetType() const {
        return attribute_ ? attribute_->GetType() : AttributeType::Red;
    }

  private:
    // 種類ごとの定義（テーブルの要素を指す・所有はしない）
    const SentanDefinition *definition_ = nullptr;

    // SENTANのType(属性)を表すコンポーネント（Sentanが所有）
    std::unique_ptr<AttributeComponent> attribute_ = nullptr;
};
