#include "WarpHole.h"
#include <functional>
#include <collider/CollisionManager.h>
#include <Particle/gpu/ParticleCSSpawner.h>
#include <common/ColliderTag.h>
#include <Input.h>


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

void WarpHole::Update()
{
    // プレイヤーがワープホール内にいる場合、Fキーが押されたらコールバックを呼び出す
    if (isPlayerInsize_ && Hagine::Input::GetInstance()->TriggerKey(DIK_F))
    {
        onEnterCallback_();
    }
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
    pCollider_->SetOnCollisionEnter(std::bind(&WarpHole::OnCollisionEnter, this, std::placeholders::_1));
    pCollider_->SetOnCollisionExit(std::bind(&WarpHole::OnCollisionExit, this, std::placeholders::_1));

    // コライダーをColliderManagerに登録
    Hagine::CollisionManager::GetInstance()->Register(pCollider_.get());
}

void WarpHole::InitializeParticleEmitter()
{
    pPortal_ = Hagine::ParticleCSSpawner::GetInstance()->Spawn("portal");
    pPortal_->SetTranslate(kPosition_);
    pPortal_->SetAuto(true);
}

void WarpHole::OnCollisionEnter(Hagine::ColliderBase* other)
{
    isPlayerInsize_ = true;
}

void WarpHole::OnCollisionExit(Hagine::ColliderBase* other)
{
    isPlayerInsize_ = false;
}