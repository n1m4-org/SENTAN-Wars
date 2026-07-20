#include "NormalAttack.h"
#include "Character/Player/Sentan/Fork.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Easing.h"
#include "Input.h"
#include <algorithm>
#include <numbers>

using namespace Hagine;

namespace {
// 通常攻撃の入力（マウス左クリック）
constexpr int32_t kAttackMouseButton = 0;
// 度からラジアンへの変換
constexpr float kDegreeToRadian = std::numbers::pi_v<float> / 180.0f;

// 構え位置（モーションによるズレが無い状態）
const Vector3 kBaseOffset{0.0f, 0.0f, 0.0f};

// 振りかぶり：一度後ろへ反ってから前へ倒れる
constexpr EasingType kTiltEasing = EasingType::InBack;
// 引き：素早く引いて、突き出す直前で溜める
constexpr EasingType kPullEasing = EasingType::OutQuad;
// 突き刺し：初速が速く、刺さった瞬間に止まる
constexpr EasingType kThrustEasing = EasingType::OutExpo;
// 戻し：ゆっくり構えへ戻る
constexpr EasingType kReturnEasing = EasingType::InOutQuad;
} // namespace

void NormalAttack::Update() {
    // 必須依存が無ければ何もしない
    if (!weapon_ || !attackState_) {
        return;
    }

    // 待機中は入力を待つ（他の攻撃が出ている間は攻撃できない）
    if (phase_ == Phase::Idle) {
        if (Input::GetInstance()->IsTriggerMouse(kAttackMouseButton) && attackState_->CanAttack()) {
            StartAttack();
        }
        return;
    }

    UpdateMotion();
}

void NormalAttack::StartAttack() {
    // 他の攻撃が割り込めないようにする
    attackState_->BeginAttack();

    // まず振りかぶってから突き刺す
    phase_ = Phase::Tilt;
    timer_ = 0.0f;
}

void NormalAttack::UpdateMotion() {
    timer_ += 1.0f;

    switch (phase_) {
    case Phase::Tilt: {
        // 構えの向きから前方へ倒しつつ、突きの反動をつけるため後方へ引く
        const float rate = CalcPhaseRate(tiltFrame_);
        ApplyTilt(ApplyEasing<float>(kTiltEasing, 0.0f, 1.0f, rate, 1.0f));
        ApplyOffset(kBaseOffset, pullOffset_, ApplyEasing<float>(kPullEasing, 0.0f, 1.0f, rate, 1.0f));
        if (rate >= 1.0f) {
            phase_ = Phase::Charge;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Charge: {
        // 突き出す前に引いた位置で止めて、溜めを見せる
        if (timer_ >= chargeFrame_) {
            phase_ = Phase::Thrust;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Thrust: {
        // 倒したまま、引いた位置から前方へ鋭く突き刺す
        const float rate = CalcPhaseRate(thrustFrame_);
        ApplyOffset(pullOffset_, thrustOffset_, ApplyEasing<float>(kThrustEasing, 0.0f, 1.0f, rate, 1.0f));
        if (rate >= 1.0f) {
            phase_ = Phase::Hold;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Hold: {
        // 突き刺さった手応えを出すため、伸びきった位置で止める
        if (timer_ >= holdFrame_) {
            phase_ = Phase::Return;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Return: {
        // 突き刺しと倒れを同時に構えへ戻す
        const float rate = CalcPhaseRate(returnFrame_);
        const float easedRate = ApplyEasing<float>(kReturnEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyOffset(thrustOffset_, kBaseOffset, easedRate);
        ApplyTilt(1.0f - easedRate);
        if (rate >= 1.0f) {
            phase_ = Phase::CoolTime;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::CoolTime: {
        // 硬直が明けたら、次の攻撃を受け付ける
        if (timer_ >= coolTimeFrame_) {
            phase_ = Phase::Idle;
            timer_ = 0.0f;
            attackState_->EndAttack();
        }
        break;
    }
    default:
        break;
    }
}

void NormalAttack::ApplyTilt(float rate) {
    // X軸で前方へ倒し、同時にZ軸でひねることで、突き刺す構えになる
    weapon_->SetMotionRotation({
        tiltAngles_.x * kDegreeToRadian * rate,
        tiltAngles_.y * kDegreeToRadian * rate,
        tiltAngles_.z * kDegreeToRadian * rate,
    });
}

void NormalAttack::ApplyOffset(const Vector3 &from, const Vector3 &to, float rate) {
    // Forkはプレイヤーの子なので、ローカルZ+がプレイヤーの前方になる
    weapon_->SetMotionOffset(from + (to - from) * rate);
}

float NormalAttack::CalcPhaseRate(float phaseFrame) const {
    // 0フレーム指定でも即座に完了するようにする
    if (phaseFrame <= 0.0f) {
        return 1.0f;
    }
    return std::min(timer_ / phaseFrame, 1.0f);
}
