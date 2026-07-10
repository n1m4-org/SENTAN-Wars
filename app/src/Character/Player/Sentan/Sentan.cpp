#include "Sentan.h"
using namespace Hagine;

void Sentan::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み
    CreateModel("Character/player/Sentan/Fork.obj");
    SetTexture("debug/white1x1.png");

    // --- コンポーネントの登録 ---
    // SENTANのType(属性)コンポーネント
    attribute_ = std::make_unique<AttributeComponent>(AttributeType::Red);
    attribute_->Init();
}

void Sentan::Update() {
    // コンポーネントの更新
    if (attribute_) {
        attribute_->Update();
    }

    // デバッグ調整したモデル位置（プレイヤー基準のトランスフォーム）を反映
    if (transform_) {
        transform_->translation_ = localPosition_;
        transform_->SetRotationEuler(localRotation_);
        transform_->scale_ = localScale_;
    }

    BaseObject::Update();
}
