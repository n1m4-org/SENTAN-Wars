#pragma once
#include "Component/Component.h"
#include "common/ColliderTag.h"
#include <functional>
#include <vector>

namespace Hagine {
class BaseObject;
class ColliderBase;
class OBBCollider;
} // namespace Hagine

/// 本体の当たり判定を持たせるコンポーネント
///
/// 自分のタグと、当たりたい相手のタグを ColliderTag で受け取る
/// 当たったときに何をするかは持たないので、必要な物が AddHitCallback で受け取る
class BodyColliderComponent : public Component {
  public:
    /// 当たった相手を受け取るコールバック
    using HitCallback = std::function<void(Hagine::ColliderBase *)>;

    /// owner      : 判定を付ける対象（追従はこのオブジェクトのワールド変換に任せる）
    /// tag        : 自分が何者かを表すタグ
    /// hitTargets : 当たりを取りたい相手のタグ
    BodyColliderComponent(Hagine::BaseObject *owner, ColliderTag::Type tag,
                          std::vector<ColliderTag::Type> hitTargets)
        : owner_(owner), tag_(tag), hitTargets_(std::move(hitTargets)) {}

    void Init() override;

    /// 当たった瞬間の通知先を追加する
    void AddHitCallback(HitCallback callback);

    /// 当たり判定本体（大きさなどはインスペクタから調整する。所有はowner側）
    Hagine::OBBCollider *GetCollider() { return collider_; }

  private:
    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::BaseObject *owner_ = nullptr;          // 判定を付ける対象
    ColliderTag::Type tag_ = ColliderTag::Type::None; // 自分のタグ
    std::vector<ColliderTag::Type> hitTargets_;    // 当たりを取りたい相手のタグ

    // ==== 状態 ====
    Hagine::OBBCollider *collider_ = nullptr; // 生成した判定（実体はownerが持つ）
    std::vector<HitCallback> hitCallbacks_;   // 当たったことを知りたい相手
};
