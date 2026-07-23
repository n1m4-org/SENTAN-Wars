#pragma once
#include "Component/Component.h"
#include "common/ColliderTag.h"
#include "type/Vector3.h"
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

    /// 大きさの既定値（中心からの半径なので、Cubeモデル1個分の2×2×2にあたる）
    /// モデルがこれと違う物は、付ける側がsizeを渡して上書きする
    static constexpr Hagine::Vector3 kDefaultSize{1.0f, 1.0f, 1.0f};

    /// owner        : 判定を付ける対象（追従はこのオブジェクトのワールド変換に任せる）
    /// tag          : 自分が何者かを表すタグ
    /// hitTargets   : 当たりを取りたい相手のタグ
    /// size         : 当たり判定の大きさ（中心からの半径）。省略すると既定値
    /// centerOffset : 判定の中心をずらす量（owner基準・向きに追従する）
    BodyColliderComponent(Hagine::BaseObject *owner, ColliderTag::Type tag,
                          std::vector<ColliderTag::Type> hitTargets,
                          const Hagine::Vector3 &size = kDefaultSize,
                          const Hagine::Vector3 &centerOffset = Hagine::Vector3{0.0f, 0.0f, 0.0f})
        : owner_(owner), tag_(tag), hitTargets_(std::move(hitTargets)), size_(size), centerOffset_(centerOffset) {}

    void Init() override;

    /// 当たった瞬間の通知先を追加する
    void AddHitCallback(HitCallback callback);

    /// 判定の有効/無効を切り替える（攻撃中だけ当たるようにするなど）
    /// Initより前に呼ばれても、生成時に反映されるようにしてある
    void SetEnabled(bool enabled);

    /// 大きさを変える（中心からの半径）
    void SetSize(const Hagine::Vector3 &size);

    /// 判定の中心をずらす（owner基準・向きに追従する）
    void SetCenterOffset(const Hagine::Vector3 &offset);

    /// 当たり判定本体（所有はowner側）
    Hagine::OBBCollider *GetCollider() { return collider_; }

  private:
    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::BaseObject *owner_ = nullptr;          // 判定を付ける対象
    ColliderTag::Type tag_ = ColliderTag::Type::None; // 自分のタグ
    std::vector<ColliderTag::Type> hitTargets_;    // 当たりを取りたい相手のタグ

    // ==== 形（生成前に指定された値も、生成時にそのまま反映する） ====
    Hagine::Vector3 size_ = kDefaultSize;              // 大きさ（中心からの半径）
    Hagine::Vector3 centerOffset_{0.0f, 0.0f, 0.0f};   // 中心のズレ
    bool isEnabled_ = true;                            // 判定を取るか

    // ==== 状態 ====
    Hagine::OBBCollider *collider_ = nullptr; // 生成した判定（実体はownerが持つ）
    std::vector<HitCallback> hitCallbacks_;   // 当たったことを知りたい相手
};
