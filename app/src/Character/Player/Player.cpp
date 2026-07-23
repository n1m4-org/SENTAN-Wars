#include "Player.h"
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Component/Attack/AttackInfo.h"
#include "Component/Attack/NormalAttack.h"
#include "Component/BodyColliderComponent.h"
#include "Component/HealthComponent.h"
#include "Component/JumpComponent.h"
#include "collider/ColliderBase.h"
#include "Component/MoveComponent.h"
#include "Component/WeaponComponent.h"
#include "common/FieldBounds.h"
using namespace Hagine;

void Player::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み
    CreatePrimitiveModel(PrimitiveType::Cube);
    SetTexture("debug/white1x1.png");

    // --- コンポーネントの登録 ---
    // HPコンポーネント（属性は持たないので相性なしの等倍で受ける）
    HealthComponent *health = AddComponent<HealthComponent>();

    // 本体の当たり判定（敵と、敵の攻撃に当たる）
    BodyColliderComponent *body = AddComponent<BodyColliderComponent>(this, ColliderTag::Type::Player,
                                                                     std::vector<ColliderTag::Type>{
                                                                         ColliderTag::Type::Enemy,
                                                                         ColliderTag::Type::EnemyBullet,
                                                                     });
    body->SetEnabled(true);

    // 当たったらダメージを受ける
    // 攻撃力はAttackRegistryから引くので、相手が何の敵かは知らなくてよい
    // 攻撃中でない敵には攻撃情報が無いため、ぶつかっただけでは減らない
    body->AddHitCallback([health](ColliderBase *other) {
        const std::string& tag = other->GetTag();
        if (tag == "EnemyBullet") 
        {
            if (const AttackInfo* attackInfo = AttackRegistry::Get(other))
            {
                health->TakeDamage(*attackInfo);
            }
        }
    });

    // 死んだらこれ以上当たらないようにする
    // TODO: ゲームオーバーの流れができたら、ここから知らせる
    health->AddDeathCallback([body]() { body->SetEnabled(false); });

    // 攻撃コンポーネントが共有する状態（攻撃の同時発動を防ぐ）
    AttackStateComponent *attackState = AddComponent<AttackStateComponent>();

    // 移動コンポーネント（基準カメラは、カメラができてからSetReferenceCameraで渡される）
    move_ = AddComponent<MoveComponent>(GetWorldTransform());

    // ジャンプコンポーネント（SENTANで跳べる回数が増えることがある）
    JumpComponent *jump = AddComponent<JumpComponent>(GetWorldTransform());

    // 武器コンポーネント（Forkを持ち、装備したSENTANの振る舞いを追加する）
    WeaponComponent *weapon = AddComponent<WeaponComponent>(this, this, attackState, jump, move_);

    // 通常攻撃コンポーネント（SENTANが無くても常に使えるので最初から持つ）
    const SentanContext normalAttackContext{GetWorldTransform(), weapon->GetFork(), attackState, jump, move_};
    AddComponent<NormalAttack>(normalAttackContext);

    // TODO: 準備フェーズができたら、そこで取得したSENTANをEquipSentanで装備する
    //       今は動作確認のため仮で装備している
    //       装備を知りたい物は、ここより前に weapon->AddEquipCallback() で登録しておく
    weapon->EquipSentan(SentanId::Sentan1);
    weapon->EquipSentan(SentanId::Sentan3);

    FlushPendingComponents();
}

void Player::SetReferenceCamera(const ViewProjection *camera) {
    if (move_) {
        move_->SetReferenceCamera(camera);
    }
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

    // 移動後の位置をフィールドの範囲内に収める（外に出られないようにする）
    // 位置は体の中心なので、体の半径ぶん内側で止めないと半身がはみ出す
    // Cubeモデルはスケール1で中心から1の大きさなので、ワールドスケールがそのまま体の大きさになる
    // 体は移動方向を向いて回るため、角が出ないように対角線を半径として使う
    FieldBounds::Clamp(GetWorldTransform()->translation_, FieldBounds::HorizontalRadius(GetWorldScale()));

    BaseObject::Update();
}

void Player::Draw(const Hagine::ViewProjection &viewProjection) {
    BaseObject::Draw(viewProjection);

    // 各コンポーネントの描画
    for (auto &component : components_) {
        component->Draw(viewProjection);
    }
}
