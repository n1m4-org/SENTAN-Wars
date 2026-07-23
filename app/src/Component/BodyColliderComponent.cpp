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

    // 本体の形（Cube）に合わせてOBBにする
    // 位置・回転はownerのワールド変換から取られるので、追従の処理は要らない
    // 大きさは既定でCubeと同じ2×2×2。合わなければインスペクタで調整すると保存される
    collider_ = owner_->AddOBBCollider(owner_->GetName() + "_BodyCollider");

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
