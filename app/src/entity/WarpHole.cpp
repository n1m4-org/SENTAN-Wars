#include "WarpHole.h"
#include <functional>
#include <collider/CollisionManager.h>
#include <Particle/gpu/ParticleCSSpawner.h>
#include <common/ColliderTag.h>

WarpHole::WarpHole(const Hagine::Vector3& pos) : kPosition_(pos)
{
    this->InitializeCollider();
    this->InitializeParticleEmitter();
}

WarpHole::~WarpHole()
{
    // パーティクルの生成の無効化 (パーティクルが消えたら自動破棄)
    Hagine::ParticleCSSpawner::GetInstance()->DespawnWhenFinished(pPortal_);
    pPortal_ = nullptr;
}

void WarpHole::InitializeCollider()
{
    // #VERIFY: ワープホール用のタグを追加
    Hagine::ColliderTagManager::GetInstance()->AddTag("WarpHole");

    // SphereColliderを作成し、ワープホールの位置に設定
    pCollider_ = std::make_unique<Hagine::SphereCollider>();
    pCollider_->SetName("WarpHole");
    pCollider_->SetRadius(1.0f); // ワープホールの半径を設定
    pCollider_->SetPositionGetter([this] { return kPosition_; });

    // タグを設定し、プレイヤーとの衝突を検知するためのマスクを追加
    {
        using namespace ColliderTag;
        pCollider_->SetTag(ToTagName(Type::WarpHole));
        pCollider_->AddCollisionMask(ToTagName(Type::Player)); // プレイヤーとの衝突を検知するためにマスクを追加
    }

    // 衝突時のコールバックを設定
    pCollider_->SetOnCollision(std::bind(&WarpHole::OnCollision, this, std::placeholders::_1));

    // コライダーをColliderManagerに登録
    Hagine::CollisionManager::GetInstance()->Register(pCollider_.get());
}

void WarpHole::InitializeParticleEmitter()
{
    pPortal_ = Hagine::ParticleCSSpawner::GetInstance()->Spawn("portal");
    pPortal_->SetTranslate(kPosition_);
    pPortal_->SetAuto(true);
}

void WarpHole::OnCollision(Hagine::ColliderBase* other)
{

}
