#include "Sentan.h"
using namespace Hagine;

void Sentan::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み（定義にモデルが無いSENTANは仮のプリミティブで描画する）
    if (definition_ && definition_->modelPath) {
        CreateModel(definition_->modelPath);
    } else {
        CreatePrimitiveModel(PrimitiveType::Cube);
    }
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

    BaseObject::Update();
}
