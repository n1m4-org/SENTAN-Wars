#include "Player.h"
#include "Component/MoveComponent.h"
using namespace Hagine;

void Player::Init(const std::string className) {
    BaseObject::Init(className);
    // モデルの読み込み（この時点で transform_ は生成済み）
    CreatePrimitiveModel(PrimitiveType::Cube);
	SetTexture("Debug/white1x1.png");

    // --- コンポーネントの登録 ---
    // 移動コンポーネント
    // 必須依存(動かす対象のTransform)をコンストラクタで注入する
    AddComponent<MoveComponent>(GetWorldTransform());

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
}
