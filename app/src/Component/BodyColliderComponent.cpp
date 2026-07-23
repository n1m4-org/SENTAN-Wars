#include "BodyColliderComponent.h"
#include "3d/Object/Base/BaseObject.h"
#include "collider/ColliderTagManager.h"

using namespace Hagine;

namespace {

/// タグを使えるようにしておく
/// ColliderTagManagerに登録されていないタグはマスクに追加しても無視されるため、
/// 使う前にここで通しておく（既に登録済みなら何も起きない）
void RegisterTag(ColliderTag::Type type) {
    ColliderTagManager::GetInstance()->AddTag(ColliderTag::ToTagName(type));
}

} // namespace

void BodyColliderComponent::Init() {
    if (!owner_) {
        return;
    }

    // 自分と相手のタグを先に通しておく（マスクの追加はタグが登録済みであることが前提）
    RegisterTag(tag_);
    for (const ColliderTag::Type target : hitTargets_) {
        RegisterTag(target);
    }

    // 向きを持つ物に付くのでOBBにする
    // 位置・回転はownerのワールド変換から取られるので、追従の処理は要らない
    collider_ = owner_->AddOBBCollider(owner_->GetName() + "_BodyCollider");

    // 形は付ける側から渡された値にする
    // モデルごとに違うため、既定値をそのまま使うのはCubeのときだけ
    collider_->SetSize(size_);
    collider_->SetPositionOffSet(centerOffset_);
    collider_->SetEnabled(isEnabled_);

    collider_->SetTag(ColliderTag::ToTagName(tag_));
    for (const ColliderTag::Type target : hitTargets_) {
        collider_->AddCollisionMask(ColliderTag::ToTagName(target));
    }

    // 当たったことを、知りたい相手へ配るだけにしておく
    // ダメージなどの中身はこのコンポーネントの担当ではない
    collider_->SetOnCollisionEnter([this](ColliderBase *other) {
        for (const HitCallback &callback : hitCallbacks_) {
            callback(other);
        }
    });
}

void BodyColliderComponent::AddHitCallback(HitCallback callback) {
    if (callback) {
        hitCallbacks_.emplace_back(std::move(callback));
    }
}

void BodyColliderComponent::SetEnabled(bool enabled) {
    isEnabled_ = enabled;
    if (collider_) {
        collider_->SetEnabled(enabled);
    }
}

void BodyColliderComponent::SetSize(const Vector3 &size) {
    size_ = size;
    if (collider_) {
        collider_->SetSize(size);
    }
}

void BodyColliderComponent::SetCenterOffset(const Vector3 &offset) {
    centerOffset_ = offset;
    if (collider_) {
        collider_->SetPositionOffSet(offset);
    }
}
