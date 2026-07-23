#pragma once
#include <type/Vector3.h>
#include <memory>
#include <collider/type/SphereCollider.h>
#include <Particle/gpu/ParticleCSEmitter.h>


/// ワープホール
class WarpHole
{
public:
    WarpHole(const Hagine::Vector3& pos);
    ~WarpHole();

    void Update();

    // ワープホールに入ったときのコールバックを設定
    void SetOnEnterCallback(std::function<void()> callback) { onEnterCallback_ = callback; }

    // プレイヤーがワープホール内にいるかどうかを取得
    bool IsPlayerInside() const { return isPlayerInsize_; }

private:
    void InitializeCollider();
    void InitializeParticleEmitter();
    void OnCollisionEnter(Hagine::ColliderBase* other);
    void OnCollisionExit(Hagine::ColliderBase* other);

    static constexpr float kMinHeight_ = 2.0f; // ワープホールの高さの最小値

    // ワープホールの位置（不変）
    const Hagine::Vector3 kPosition_;

    // コライダーとパーティクルエミッターのポインタ
    std::unique_ptr<Hagine::SphereCollider> pCollider_ = nullptr;

    // パーティクルエミッターのポインタ
    Hagine::ParticleCSEmitter* pPortal_ = nullptr;

    // プレイヤーがワープホール内にいるかどうかのフラグ
    bool isPlayerInsize_ = false;

    // ワープホールに入ったときのコールバック関数
    std::function<void()> onEnterCallback_ = nullptr;
};