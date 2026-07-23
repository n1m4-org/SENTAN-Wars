#include "WeaponComponent.h"
#include "3d/Object/Base/BaseObject.h"
#include "Character/Player/Sentan/SentanContext.h"
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
}

void WeaponComponent::AddEquipCallback(EquipCallback callback) {
    if (callback) {
        equipCallbacks_.emplace_back(std::move(callback));
    }
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

    // このSENTANが解禁する振る舞いを、所有者のコンポーネントとして追加する
    // 振る舞いが未実装のSENTANは createComponent が nullptr なので、何も増えない
    const SentanDefinition &definition = sentan->GetDefinition();
    Component *unlockedComponent = nullptr;
    if (definition.createComponent && container_) {
        const SentanContext context{
            owner_ ? owner_->GetWorldTransform() : nullptr,
            fork_.get(),
            attackState_,
            jump_,
        };
        unlockedComponent = container_->AddComponent(definition.createComponent(context));
    }

    // 装備し終えてから知らせる（受け取った側がSentanや解禁された振る舞いを触れる状態にしておく）
    const SentanEquipEvent event{id, sentan, &definition, fork_->GetSentanCount() - 1, unlockedComponent};
    for (const EquipCallback &callback : equipCallbacks_) {
        callback(event);
    }

    return true;
}

void WeaponComponent::Draw(const ViewProjection &viewProjection) {
    // 子は自動では描画されないため、武器を描画する（SENTANはForkが描画する）
    if (fork_) {
        fork_->Draw(viewProjection);
    }
}
