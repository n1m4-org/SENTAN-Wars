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
#include "audio/SoundPlayer.h"
#include "common/FieldBounds.h"
using namespace Hagine;

void Player::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み
    CreatePrimitiveModel(PrimitiveType::Cube);
    SetTexture("debug/white1x1.png");

    // --- コンポーネントの登録 ---
    // HPコンポーネント（属性は持たないので相性なしの等倍で受ける）
    // 外からHPを読めるよう、生成したものを覚えておく
    HealthComponent *health = AddComponent<HealthComponent>();
    health_ = health;

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

    // HPが尽きた後もダメージはHealthComponent側で止まるため、ここで判定は切らない
    // 判定を切ると体そのものが無くなり、ポータルなど「触れたか」を見ている物にも当たらなくなる
    // TODO: ゲームオーバーの流れができたら、ここから知らせる

    // 攻撃コンポーネントが共有する状態（攻撃の同時発動を防ぐ）
    AttackStateComponent *attackState = AddComponent<AttackStateComponent>();

    // 武器を振った音を鳴らす
    // 判定が開くのは刃が動き始める瞬間なので、どの攻撃でもここが「振った」タイミングになる
    // （個々の攻撃コンポーネントは音を知らないままでいられる）
    attackState->AddHitWindowCallback([](bool isOpen, const HitWindow &) {
        if (isOpen) {
            SoundPlayer::GetInstance()->PlaySwing();
        }
    });

    // 移動コンポーネント（基準カメラは、カメラができてからSetReferenceCameraで渡される）
    move_ = AddComponent<MoveComponent>(GetWorldTransform());

    // ジャンプコンポーネント（SENTANで跳べる回数が増えることがある）
    JumpComponent *jump = AddComponent<JumpComponent>(GetWorldTransform());

    // 武器コンポーネント（Forkを持ち、装備したSENTANの振る舞いを追加する）
    // SENTANの装備を取り次ぐために覚えておく
    weapon_ = AddComponent<WeaponComponent>(this, this, attackState, jump, move_);

    // 通常攻撃コンポーネント（SENTANが無くても常に使えるので最初から持つ）
    const SentanContext normalAttackContext{GetWorldTransform(), weapon_->GetFork(), attackState, jump, move_};
    AddComponent<NormalAttack>(normalAttackContext);

    // SENTANはここでは装備しない
    // 何を付けるかは準備フェーズなど外側が決めるので、Player::EquipSentan で渡してもらう
    // （装備を知りたい物は、装備より前に weapon_->AddEquipCallback() で登録しておく）

    FlushPendingComponents();
}

bool Player::EquipSentan(SentanId id) {
    return weapon_ ? weapon_->EquipSentan(id) : false;
}

void Player::UnequipAllSentan() {
    if (weapon_) {
        weapon_->UnequipAllSentan();
    }
}

size_t Player::GetSentanCount() const {
    const Fork *fork = weapon_ ? weapon_->GetFork() : nullptr;
    return fork ? fork->GetSentanCount() : 0;
}

size_t Player::GetMaxSentanCount() {
    return Fork::kMaxSentanCount;
}

void Player::SetReferenceCamera(const ViewProjection *camera) {
    if (move_) {
        move_->SetReferenceCamera(camera);
    }
}

float Player::GetHp() const {
    return health_ ? health_->GetHp() : 0.0f;
}

float Player::GetMaxHp() const {
    return health_ ? health_->GetMaxHp() : 0.0f;
}

bool Player::IsDead() const {
    return health_ ? health_->IsDead() : false;
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

void Player::RemoveComponent(Component *component) {
    if (!component) {
        return;
    }

    // 実際に消すのは次の更新の頭
    // 更新中に消すと、回している最中のcomponents_が動いてしまう
    removedComponents_.emplace_back(component);
}

void Player::FlushPendingComponents() {
    for (auto &component : pendingComponents_) {
        components_.emplace_back(std::move(component));
    }
    pendingComponents_.clear();
}

void Player::FlushRemovedComponents() {
    if (removedComponents_.empty()) {
        return;
    }

    for (Component *removed : removedComponents_) {
        std::erase_if(components_, [removed](const std::unique_ptr<Component> &component) {
            return component.get() == removed;
        });
        // 登録される前に外された場合もあるので、待ち行列からも消す
        std::erase_if(pendingComponents_, [removed](const std::unique_ptr<Component> &component) {
            return component.get() == removed;
        });
    }

    removedComponents_.clear();
}

void Player::Update() {
    // 前フレームまでに外されたコンポーネントをここで破棄する（追加より先）
    FlushRemovedComponents();

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
