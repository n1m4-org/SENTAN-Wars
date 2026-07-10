#include "Player.h"
#include "Component/MoveComponent.h"
#include "Component/SentanComponent.h"
using namespace Hagine;

void Player::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み
    CreatePrimitiveModel(PrimitiveType::Cube);
	SetTexture("debug/white1x1.png");

    // --- コンポーネントの登録 ---
    // 移動コンポーネント
    AddComponent<MoveComponent>(GetWorldTransform());

    // SENTANコンポーネント
    AddComponent<SentanComponent>(this);

    // 登録済みコンポーネントの初期化
    for (auto &component : components_) {
        component->Init();
    }
}

void Player::Update() {
    // 各コンポーネントの更新
    for (auto &component : components_) {
        component->Update();
    }

    BaseObject::Update();
}

void Player::Draw(const Hagine::ViewProjection &viewProjection) {
    BaseObject::Draw(viewProjection);

    // 各コンポーネントの描画
    for (auto &component : components_) {
        component->Draw(viewProjection);
    }
}
