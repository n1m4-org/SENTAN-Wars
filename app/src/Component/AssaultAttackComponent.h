#pragma once
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"
#include <3d/Object/Base/BaseObject.h>

#include <memory>

namespace Hagine
{
    class WorldTransform;
    class ViewProjection;
} // namespace Hagine

class AssaultAttackComponent : public Component
{
public:
    /// 必要な物はコンストラクタで受け取る

    /// <summary>
    /// 
    /// </summary>
    /// <param name="transform">所有者のトランスフォーム</param>
    /// <param name="attackRange">所有者の攻撃範囲</param>
    /// <param name="transform">攻撃判定オブジェクトのトランスフォーム</param>
    /// <param name="target">ターゲットの座標</param>
    /// <param name="radius">ターゲットの半径</param>
    explicit AssaultAttackComponent(Hagine::WorldTransform* transform, float* attackRange, Hagine::WorldTransform* attackTransform, Hagine::Vector3* target, float* radius)
        : transform_(transform), attackRange_(attackRange), attackTransform_(attackTransform), target_(target), radius_(radius)
    {}

    void Init() override { Init("AssaultEnemy"); }
    void Init(const std::string& ownerName);

    void Update() override;

    bool IsActive() const { return isActive_; }

private:
    // GameParameterの登録先となるデバッグエントリ
    EnableDebug("AssaultAttack");

    // ==== 注入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform* transform_ = nullptr;
    Hagine::WorldTransform* attackTransform_ = nullptr;
    Hagine::Vector3* target_ = nullptr;
    float* attackRange_ = nullptr;
    float* radius_ = nullptr;

    //
    std::unique_ptr<Hagine::BaseObject> AoEObject_ = nullptr;

    // ==== 保持用パラメータ ====
    Hagine::Vector3 startPos_{ 0.0f, 0.0f, 0.0f };
    Hagine::Vector3 endPos_{ 0.0f, 0.0f, 0.0f };
    Hagine::Vector3 scale_{ 1.0f, 1.0f, 1.0f };
    bool isActive_ = false;

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(float, attackTimer_, 0.0f);
    GameParameter(float, attackTime_, 1.2f);
    GameParameter(float, coolTime_, 4.0f);
};
