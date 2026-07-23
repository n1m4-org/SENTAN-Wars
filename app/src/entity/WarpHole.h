#pragma once
#include <type/Vector3.h>
#include <memory>
#include <collider/type/SphereCollider.h>
#include <Particle/gpu/ParticleCSEmitter.h>


/// ワープホール
class WarpHole
{
public:
    WarpHole(const Hagine::Vector3& pos) : kPosition_(pos);
    ~WarpHole();

    void Update();

private:
    void InitializeCollider();
    void InitializeParticleEmitter();
    void OnCollision(Hagine::ColliderBase* other);

    // ワープホールの位置（不変）
    const Hagine::Vector3 kPosition_;
    
    // コライダーとパーティクルエミッターのポインタ
    std::unique_ptr<Hagine::SphereCollider> pCollider_ = nullptr;

    // パーティクルエミッターのポインタ
    Hagine::ParticleCSEmitter* pPortal_ = nullptr;
};