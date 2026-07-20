#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"

namespace Hagine
{
    class WorldTransform;
    class ViewProjection;
} // namespace Hagine

//  TODO: 攻撃中は押し戻し処理をオフにする

class LeapAttackComponent : public Component
{
public:
    /// 必要な物はコンストラクタで受け取る

    /// <summary>
    /// 
    /// </summary>
    /// <param name="transform">所有者のトランスフォーム</param>
    /// <param name="attackRange">所有者の攻撃範囲</param>
    /// <param name="target">ターゲットの座標</param>
    /// <param name="radius">ターゲットの半径</param>
    explicit LeapAttackComponent(Hagine::WorldTransform* transform, float* attackRange, Hagine::Vector3* target, float* radius)
        : transform_(transform), attackRange_(attackRange), target_(target), radius_(radius)
    {}

    void Update() override;

    bool IsActive() const { return isActive_; }

private:
    // GameParameterの登録先となるデバッグエントリ
    EnableDebug("LeapAttack");

    // ==== 注入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform* transform_ = nullptr;
    Hagine::Vector3* target_ = nullptr;
    float* attackRange_ = nullptr;
    float* radius_ = nullptr;

    // ==== 保持用パラメータ ====
    Hagine::Vector3 startPos_{ 0.0f, 0.0f, 0.0f };
    Hagine::Vector3 endPos_{ 0.0f, 0.0f, 0.0f };
    Hagine::Vector3 scale_{ 1.0f, 1.0f, 1.0f };
    bool isActive_ = false;

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(float, attackTimer_, 0.0f);
    GameParameter(float, attackTime_, 1.0f);
    GameParameter(float, coolTime_, 4.5f);
};
