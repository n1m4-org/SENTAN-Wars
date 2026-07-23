#include "Fork.h"
#include "type/Quaternion.h"
using namespace Hagine;

void Fork::Init(const std::string className) {
    BaseObject::Init(className);

    // モデルの読み込み（Forkは1本だけなので、ここでしか読まない）
    CreateModel("Character/player/Sentan/Fork.obj");
    SetTexture("debug/white1x1.png");
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

    auto sentan = std::make_unique<Sentan>(*definition);
    sentan->Init("Sentan");

    // Forkの子にして追従させる（更新・ワールド変換は親子関係で行われる）
    sentan->SetParent(this);

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
