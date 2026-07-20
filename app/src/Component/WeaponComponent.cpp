#include "WeaponComponent.h"
#include "3d/Object/Base/BaseObject.h"
#include "Component/Attack/AttackContext.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Component/ComponentContainer.h"

using namespace Hagine;

void WeaponComponent::Init() {
    // 武器本体を作る（Forkは1本だけ）
    fork_ = std::make_unique<Fork>();
    fork_->Init("Fork");

    // 装備先に追従させる（更新・ワールド変換は親子関係で行われる）
    if (owner_) {
        fork_->SetParent(owner_);
    }

    // TODO: 準備フェーズができたら、そこで取得したSENTANをEquipSentanで装備する
    //       今は動作確認のため仮で装備している
    EquipSentan(SentanId::Sentan1);
    EquipSentan(SentanId::Sentan2);
}

bool WeaponComponent::EquipSentan(SentanId id) {
    if (!fork_) {
        return false;
    }

    // Forkにくっつける（最大2つを超えるとnullptrが返る）
    Sentan *sentan = fork_->AttachSentan(id);
    if (!sentan) {
        return false;
    }

    // このSENTANが解禁する攻撃を、所有者の振る舞いとして追加する
    // 攻撃が未実装のSENTANは createAttack が nullptr なので、何も増えない
    const SentanDefinition &definition = sentan->GetDefinition();
    if (definition.createAttack && container_) {
        const AttackContext context{
            owner_ ? owner_->GetWorldTransform() : nullptr,
            fork_.get(),
            attackState_,
        };
        container_->AddComponent(definition.createAttack(context));
    }

    return true;
}

void WeaponComponent::Draw(const ViewProjection &viewProjection) {
    // 子は自動では描画されないため、武器を描画する（SENTANはForkが描画する）
    if (fork_) {
        fork_->Draw(viewProjection);
    }
}
