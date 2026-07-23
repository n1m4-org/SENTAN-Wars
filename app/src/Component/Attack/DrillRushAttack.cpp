#include "DrillRushAttack.h"
#include "3d/Transform/WorldTransform.h"
#include "Character/Player/Sentan/Fork.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Easing.h"
#include "Input.h"
#include "myMath.h"
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace Hagine;

namespace {
// 突進の入力（マウス右クリック）
constexpr int32_t kRushMouseButton = 1;
// 度からラジアンへの変換
constexpr float kDegreeToRadian = std::numbers::pi_v<float> / 180.0f;
// 1回転
constexpr float kFullTurn = std::numbers::pi_v<float> * 2.0f;

// 構え位置（モーションによるズレが無い状態）
const Vector3 kBaseOffset{0.0f, 0.0f, 0.0f};

// 倒れ：構えから素早く倒れる
constexpr EasingType kTiltEasing = EasingType::OutQuad;
// 引き：素早く引いて、回し始める直前で溜める
constexpr EasingType kPullEasing = EasingType::OutQuad;
// 突進：初速が速く、終わりへ向けて減速する
constexpr EasingType kRushEasing = EasingType::OutQuart;
// 戻し：ゆっくり構えへ戻る
constexpr EasingType kReturnEasing = EasingType::InOutQuad;
} // namespace

void DrillRushAttack::Update() {
    // 必須依存が無ければ何もしない
    if (!weapon_ || !attackState_ || !ownerTransform_) {
        return;
    }

    // 待機中は入力を待つ（他の攻撃が出ている間は攻撃できない）
    if (phase_ == Phase::Idle) {
        if (Input::GetInstance()->IsTriggerMouse(kRushMouseButton) && attackState_->CanAttack()) {
            StartAttack();
        }
        return;
    }

    UpdateMotion();
}

void DrillRushAttack::StartAttack() {
    // 他の攻撃が割り込めないようにする
    attackState_->BeginAttack();

    // まず倒しながら回し始める
    phase_ = Phase::Tilt;
    timer_ = 0.0f;
    spinAngle_ = 0.0f;
}

void DrillRushAttack::StartRush() {
    // 突進中に向きを変えられると軌道が曲がるので、開始時の向きで固定する
    rushStartPosition_ = ownerTransform_->translation_;
    rushDirection_ = CalcForward();
}

void DrillRushAttack::UpdateMotion() {
    timer_ += 1.0f;

    switch (phase_) {
    case Phase::Tilt: {
        // 構えの向きから前方へ倒しつつ、突進の反動をつけるため後方へ引く
        const float rate = CalcPhaseRate(tiltFrame_);
        ApplyTilt(ApplyEasing<float>(kTiltEasing, 0.0f, 1.0f, rate, 1.0f));
        ApplyOffset(kBaseOffset, pullOffset_, ApplyEasing<float>(kPullEasing, 0.0f, 1.0f, rate, 1.0f));
        if (rate >= 1.0f) {
            phase_ = Phase::SpinUp;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::SpinUp: {
        // 倒しきってから回し始める（引いた位置で回転を溜めてから突進に入る）
        AdvanceSpin();
        if (timer_ >= spinUpFrame_) {
            phase_ = Phase::Rush;
            timer_ = 0.0f;
            StartRush();

            // ここから回しながら突っ込むので、当たり判定を開ける
            attackState_->BeginHit({atk_, hitScale_});
        }
        break;
    }
    case Phase::Rush: {
        // 回し続けたまま武器を前方へ突き出し、本体を滑らせる
        const float rate = CalcPhaseRate(rushFrame_);
        const float easedRate = ApplyEasing<float>(kRushEasing, 0.0f, 1.0f, rate, 1.0f);
        AdvanceSpin();
        ApplyOffset(pullOffset_, thrustOffset_, easedRate);
        ApplyRush(easedRate);
        if (rate >= 1.0f) {
            phase_ = Phase::Return;
            timer_ = 0.0f;

            // 突進が終わったので、当たり判定を閉じる
            attackState_->EndHit();

            // 自転を途中で切ると見た目が飛ぶので、次に回り切る位置まで回してから止める
            spinStartAngle_ = spinAngle_;
            spinEndAngle_ = std::ceil(spinAngle_ / kFullTurn) * kFullTurn;
        }
        break;
    }
    case Phase::Return: {
        // 突き出し・倒れ・自転を同時に構えへ戻す
        const float rate = CalcPhaseRate(returnFrame_);
        const float easedRate = ApplyEasing<float>(kReturnEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyOffset(thrustOffset_, kBaseOffset, easedRate);
        ApplyTilt(1.0f - easedRate);
        spinAngle_ = ApplyEasing<float>(kReturnEasing, spinStartAngle_, spinEndAngle_, rate, 1.0f);
        weapon_->SetMotionSpin(spinAngle_);
        if (rate >= 1.0f) {
            phase_ = Phase::CoolTime;
            timer_ = 0.0f;

            // 回り切った位置は構えと同じ向きなので、0に戻しても見た目は変わらない
            spinAngle_ = 0.0f;
            weapon_->SetMotionSpin(0.0f);
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

void DrillRushAttack::ApplyTilt(float rate) {
    weapon_->SetMotionRotation({
        tiltAngles_.x * kDegreeToRadian * rate,
        tiltAngles_.y * kDegreeToRadian * rate,
        tiltAngles_.z * kDegreeToRadian * rate,
    });
}

void DrillRushAttack::ApplyOffset(const Vector3 &from, const Vector3 &to, float rate) {
    // Forkはプレイヤーの子なので、ローカルZ+がプレイヤーの前方になる
    weapon_->SetMotionOffset(from + (to - from) * rate);
}

void DrillRushAttack::AdvanceSpin() {
    spinAngle_ += spinSpeed_ * kDegreeToRadian;
    weapon_->SetMotionSpin(spinAngle_);
}

void DrillRushAttack::ApplyRush(float rate) {
    // 開始位置からの絶対位置で置くことで、突進中は移動入力に流されない
    // 高さは触らないので、ジャンプ中でも落下は続く
    const Vector3 moved = rushStartPosition_ + rushDirection_ * (rushDistance_ * rate);
    ownerTransform_->translation_.x = moved.x;
    ownerTransform_->translation_.z = moved.z;
}

Vector3 DrillRushAttack::CalcForward() const {
    // 本体のローカルZ+が正面。水平方向だけ取り出す
    Vector3 forward = TransformNormal({0.0f, 0.0f, 1.0f}, ownerTransform_->matWorld_);
    forward.y = 0.0f;
    if (forward.LengthSq() <= 0.0f) {
        return {0.0f, 0.0f, 1.0f};
    }
    return forward.Normalize();
}

float DrillRushAttack::CalcPhaseRate(float phaseFrame) const {
    // 0フレーム指定でも即座に完了するようにする
    if (phaseFrame <= 0.0f) {
        return 1.0f;
    }
    return std::min(timer_ / phaseFrame, 1.0f);
}
