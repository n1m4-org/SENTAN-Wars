#include "Fork.h"
#include "Component/Attack/AttackInfo.h"
#include "type/Quaternion.h"
#include <string>
using namespace Hagine;

Fork::~Fork() {
    // 攻撃情報はコライダーのアドレスで引かれるため、消える前に外しておく
    SetAttackColliderEnabled(false);
}

void Fork::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み（Forkは1本だけなので、ここでしか読まない）
    CreateModel("Character/player/Sentan/Fork.obj");
    SetTexture("debug/white1x1.png");

    // --- コンポーネントの登録 ---
    // 攻撃の当たり判定（SENTANが無くても通常攻撃で当たるので、Fork自身も持つ）
    collider_ = std::make_unique<BodyColliderComponent>(
        this, ColliderTag::Type::PlayerAttack, std::vector<ColliderTag::Type>{ColliderTag::Type::Enemy},
        colliderSize_, colliderOffset_);
    collider_->Init();

    // 振っていない間は当たらない
    SetAttackColliderEnabled(false);
}

void Fork::SetAttackColliderEnabled(bool enabled, const HitWindow &hit) {
    isAttackColliderEnabled_ = enabled;
    hit_ = hit;

    // Fork本体には属性が無いので相性なしの扱いにする
    ApplyAttackCollider(collider_.get(), colliderSize_, AttributeType::Red);

    // SENTANは自分の属性で当たる（属性相性は当てられた側が見る）
    // 元の大きさは種類ごとの定義から取るので、モデルが違っても同じ倍率で広がる
    for (auto &sentan : sentans_) {
        ApplyAttackCollider(sentan->GetCollider(), sentan->GetDefinition().colliderSize, sentan->GetType());
    }
}

void Fork::ApplyAttackCollider(BodyColliderComponent *collider, const Vector3 &baseSize, AttributeType attribute) {
    if (!collider) {
        return;
    }

    collider->SetEnabled(isAttackColliderEnabled_);

    // 武器の形そのままだと細くて当たらないので、攻撃ごとの倍率で広げる
    collider->SetSize(baseSize * GetHitSizeScale());

    ColliderBase *raw = collider->GetCollider();
    if (!raw) {
        return;
    }

    // 判定を開けている間だけ攻撃情報を持たせる
    // 当てられた側は AttackRegistry から引くので、こちらは相手を知らなくてよい
    if (isAttackColliderEnabled_) {
        AttackRegistry::Register(raw, AttackInfo{hit_.damage, attribute, attacker_});
    } else {
        AttackRegistry::Unregister(raw);
    }
}

Sentan *Fork::AttachSentan(SentanId id) {
    // SENTANは最大2つまで
    if (sentans_.size() >= kMaxSentanCount) {
        return nullptr;
    }

    // 種類ごとの定義（モデル・解禁する攻撃）を決める
    const SentanDefinition *definition = FindSentanDefinition(id);
    if (!definition) {
        return nullptr;
    }

    // 種類ごとに名前を分ける
    // 当たり判定などの設定はこの名前で保存されるため、同じ名前だと種類同士で混ざってしまう
    auto sentan = std::make_unique<Sentan>(*definition);
    sentan->Init("Sentan" + std::to_string(static_cast<int>(id) + 1));

    // Forkの子にして追従させる（更新・ワールド変換は親子関係で行われる）
    sentan->SetParent(this);

    // 途中でくっついても判定の開閉がずれないよう、今の状態に合わせる
    ApplyAttackCollider(sentan->GetCollider(), definition->colliderSize, sentan->GetType());

    Sentan *raw = sentan.get();
    sentans_.emplace_back(std::move(sentan));
    return raw;
}

void Fork::Update() {
    // 構えに攻撃モーションのズレを足したものが、今フレームの位置と回転になる
    if (transform_) {
        transform_->translation_ = basePosition_ + motionOffset_;

        // 回転は左から順に適用されるため、自転を先に置くと自分の軸まわりに回り、
        // その結果を倒れが運ぶ（逆にすると親の軸で振り回されてしまう）
        const Quaternion tilt = Quaternion::FromEulerAngles(baseRotation_ + motionRotation_);
        const Quaternion spin = Quaternion::FromAxisAngle(spinAxis_.Normalize(), motionSpin_);
        transform_->SetRotationQuaternion(spin * tilt);

        transform_->scale_ = baseScale_;
    }

    // 当たり判定の形も毎フレーム合わせる（構え位置と同じく、動かしながら調整できるようにする）
    // 攻撃中は、その攻撃の倍率で広げた大きさになる
    if (collider_) {
        collider_->SetSize(colliderSize_ * GetHitSizeScale());
        collider_->SetCenterOffset(colliderOffset_);
    }

    // 装着中のSENTANをスロット位置へ配置する
    for (size_t i = 0; i < sentans_.size(); ++i) {
        WorldTransform *sentanTransform = sentans_[i]->GetWorldTransform();
        if (!sentanTransform) {
            continue;
        }
        sentanTransform->translation_ = (i == 0) ? sentanSlot0_ : sentanSlot1_;
        sentanTransform->scale_ = sentanScale_;
    }

    BaseObject::Update();
}

void Fork::Draw(const ViewProjection &viewProjection) {
    BaseObject::Draw(viewProjection);

    // 装着中のSENTANを描画する
    for (auto &sentan : sentans_) {
        sentan->Draw(viewProjection);
    }
}
