#include "Player.h"
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Component/Attack/NormalAttack.h"
#include "Component/JumpComponent.h"
#include "Component/MoveComponent.h"
#include "Component/WeaponComponent.h"
using namespace Hagine;

void Player::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み
    CreatePrimitiveModel(PrimitiveType::Cube);
    SetTexture("debug/white1x1.png");

    // --- コンポーネントの登録 ---
    // 攻撃コンポーネントが共有する状態（攻撃の同時発動を防ぐ）
    AttackStateComponent *attackState = AddComponent<AttackStateComponent>();

    // 移動コンポーネント
    AddComponent<MoveComponent>(GetWorldTransform());

    // ジャンプコンポーネント（SENTANで跳べる回数が増えることがある）
    JumpComponent *jump = AddComponent<JumpComponent>(GetWorldTransform());

    // 武器コンポーネント（Forkを持ち、装備したSENTANの振る舞いを追加する）
    WeaponComponent *weapon = AddComponent<WeaponComponent>(this, this, attackState, jump);

    // 通常攻撃コンポーネント（SENTANが無くても常に使えるので最初から持つ）
    const SentanContext normalAttackContext{GetWorldTransform(), weapon->GetFork(), attackState, jump};
    AddComponent<NormalAttack>(normalAttackContext);

    // TODO: 準備フェーズができたら、そこで取得したSENTANをEquipSentanで装備する
    //       今は動作確認のため仮で装備している
    //       装備を知りたい物は、ここより前に weapon->AddEquipCallback() で登録しておく
    weapon->EquipSentan(SentanId::Sentan4);
    weapon->EquipSentan(SentanId::Sentan2);

    FlushPendingComponents();
}

Component *Player::AddComponent(std::unique_ptr<Component> component) {
    if (!component) {
        return nullptr;
    }

    component->Init();

    Component *raw = component.get();
    pendingComponents_.emplace_back(std::move(component));
    return raw;
}

void Player::FlushPendingComponents() {
    for (auto &component : pendingComponents_) {
        components_.emplace_back(std::move(component));
    }
    pendingComponents_.clear();
}

void Player::Update() {
    // 前フレームまでに追加されたコンポーネントをここで登録する
    FlushPendingComponents();

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
