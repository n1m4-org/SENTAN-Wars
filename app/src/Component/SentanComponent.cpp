#include "SentanComponent.h"
#include "3d/Object/Base/BaseObject.h"
using namespace Hagine;

void SentanComponent::Init() {
    // --- SENTAN(武器)の装備（最大2つ・左右に配置） ---
    EquipSentan(AttributeType::Red, {kSentanEquipOffsetX, 0.0f, 0.0f});
    EquipSentan(AttributeType::Blue, {-kSentanEquipOffsetX, 0.0f, 0.0f});
}

Sentan *SentanComponent::EquipSentan(AttributeType type, const Vector3 &localOffset) {
    // SENTANは最大2つまで持てる
    if (sentans_.size() >= kMaxSentanCount) {
        return nullptr;
    }

    auto sentan = std::make_unique<Sentan>();
    sentan->Init("Sentan");
    sentan->SetType(type);

    // 装備先に追従させる（更新・ワールド変換は親子関係で行われる）
    if (owner_) {
        sentan->SetParent(owner_);
    }
    sentan->SetLocalPosition(localOffset);

    Sentan *raw = sentan.get();
    sentans_.emplace_back(std::move(sentan));
    return raw;
}

void SentanComponent::Draw(const ViewProjection &viewProjection) {
    // 装備中のSENTANを描画する
    for (auto &sentan : sentans_) {
        sentan->Draw(viewProjection);
    }
}
