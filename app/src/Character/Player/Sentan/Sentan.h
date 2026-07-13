#pragma once
#include "3d/Object/Base/BaseObject.h"
#include "Component/AttributeComponent.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"
#include <memory>

/// プレイヤーの武器(SENTAN)
/// SENTANは Type(属性)、Level、Atk を持つ
class Sentan : public Hagine::BaseObject {
  public:
    void Init(const std::string className) override;

    void Update() override;

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

    /// プレイヤーからの位置を設定する
    void SetLocalPosition(const Hagine::Vector3 &position) { localPosition_ = position; }

    /// プレイヤーからのスケールを設定する
    void SetLocalScale(const Hagine::Vector3 &scale) { localScale_ = scale; }

  private:
    // GameParameterの登録先
    EnableDebug("Sentan");

    // ==== モデルの位置調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(Hagine::Vector3, localPosition_, (Hagine::Vector3{0.0f, 0.0f, 0.0f})); // 位置（装備時にPlayerが設定）
    GameParameter(Hagine::Vector3, localRotation_, (Hagine::Vector3{0.0f, 0.0f, 0.0f})); // 回転
    GameParameter(Hagine::Vector3, localScale_, (Hagine::Vector3{1.0f, 1.0f, 1.0f}));    // スケール

    // SENTANのType(属性)を表すコンポーネント（Sentanが所有）
    std::unique_ptr<AttributeComponent> attribute_ = nullptr;
};
