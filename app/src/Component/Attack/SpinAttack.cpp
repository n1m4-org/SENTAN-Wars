#include "SpinAttack.h"
#include "3d/Transform/WorldTransform.h"
#include "Character/Player/Sentan/Fork.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Component/MoveComponent.h"
#include "Easing.h"
#include "Input.h"
#include <algorithm>
#include <numbers>

using namespace Hagine;

namespace {
// 回転攻撃の入力キー
constexpr BYTE kSpinKey = DIK_V;
// 度からラジアンへの変換
constexpr float kDegreeToRadian = std::numbers::pi_v<float> / 180.0f;
// 1周
constexpr float kFullTurn = std::numbers::pi_v<float> * 2.0f;

// 構え位置（モーションによるズレが無い状態）
const Vector3 kBaseOffset{0.0f, 0.0f, 0.0f};
// 構えの角度（度）
const Vector3 kBaseAngles{0.0f, 0.0f, 0.0f};

// 構えへ入る：素早く横へ倒す
constexpr EasingType kWindupEasing = EasingType::OutQuad;
// 回転：溜めてから一気に回り、回り終わりで減速する
constexpr EasingType kSpinEasing = EasingType::InOutQuad;
// 戻し：ゆっくり構えへ戻る
constexpr EasingType kReturnEasing = EasingType::InOutQuad;
} // namespace

SpinAttack::~SpinAttack() {
    // 回転中に消えると移動が鈍いままになるので、必ず戻す
    SetMoveSpeedScale(1.0f);
}

void SpinAttack::Update() {
    // 必須依存が無ければ何もしない
    if (!weapon_ || !attackState_ || !ownerTransform_) {
        return;
    }

    // 待機中は入力を待つ（他の攻撃が出ている間は攻撃できない）
    if (phase_ == Phase::Idle) {
        if (Input::GetInstance()->TriggerKey(kSpinKey) && attackState_->CanAttack()) {
            StartAttack();
        }
        return;
    }

    UpdateMotion();
}

void SpinAttack::StartAttack() {
    // 他の攻撃が割り込めないようにする
    attackState_->BeginAttack();

    phase_ = Phase::Windup;
    timer_ = 0.0f;

    // 他の攻撃が回した自転が残っていても、回転攻撃は自転させないので戻しておく
    weapon_->SetMotionSpin(0.0f);

    // 回っている間は移動を鈍らせる（止めず、遅くするだけ）
    SetMoveSpeedScale(moveSpeedScale_);
}

void SpinAttack::StartSpin() {
    // 回した角度はここからの相対で足すので、回り始めの向きを控える
    // 周回数は整数なので、回りきると元の向きへ戻る
    startRotation_ = ownerTransform_->quateRotation_;

    phase_ = Phase::Spin;
    timer_ = 0.0f;
    hitIndex_ = 0;

    attackState_->BeginHit({atk_, hitScale_});
}

void SpinAttack::UpdateMotion() {
    timer_ += 1.0f;

    switch (phase_) {
    case Phase::Windup: {
        // 構えから武器を横へ倒し、前へ突き出して回る半径を作る
        const float rate = CalcPhaseRate(windupFrame_);
        const float easedRate = ApplyEasing<float>(kWindupEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(kBaseAngles, holdAngles_, easedRate);
        ApplyOffset(kBaseOffset, holdOffset_, easedRate);
        if (rate >= 1.0f) {
            StartSpin();
        }
        break;
    }
    case Phase::Spin: {
        // 武器は構えたまま、本体ごと回る
        UpdateSpin();
        break;
    }
    case Phase::Finish: {
        // 回り終わった姿勢で止めて、薙ぎ終わりを見せる
        if (timer_ >= finishFrame_) {
            phase_ = Phase::Return;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Return: {
        // 横へ構えた姿勢から構えへ戻す
        const float rate = CalcPhaseRate(returnFrame_);
        const float easedRate = ApplyEasing<float>(kReturnEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(holdAngles_, kBaseAngles, easedRate);
        ApplyOffset(holdOffset_, kBaseOffset, easedRate);
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
            hitIndex_ = 0;
            attackState_->EndAttack();

            // 移動の速度を元に戻す
            SetMoveSpeedScale(1.0f);
        }
        break;
    }
    default:
        break;
    }
}

void SpinAttack::UpdateSpin() {
    const int32_t turns = GetSpinTurns();
    const float rate = CalcPhaseRate(spinFrame_);
    const float easedRate = ApplyEasing<float>(kSpinEasing, 0.0f, 1.0f, rate, 1.0f);

    // 回りきったところで整数周になるので、元の向きへ戻って終わる
    const float turnsDone = easedRate * static_cast<float>(turns);
    ApplyOwnerYaw(turnsDone * kFullTurn);

    // 1周ごとに判定を開け直す
    // 開きっぱなしだと当たり始めが1回しか起きず、何周回しても1回しか当たらない
    const int32_t index = (std::min)(static_cast<int32_t>(turnsDone), turns - 1);
    if (index != hitIndex_) {
        hitIndex_ = index;
        attackState_->EndHit();
        attackState_->BeginHit({atk_, hitScale_});
    }

    if (rate >= 1.0f) {
        phase_ = Phase::Finish;
        timer_ = 0.0f;
        attackState_->EndHit();
    }
}

void SpinAttack::ApplyRotation(const Vector3 &fromDegrees, const Vector3 &toDegrees, float rate) {
    const Vector3 degrees = fromDegrees + (toDegrees - fromDegrees) * rate;
    weapon_->SetMotionRotation(degrees * kDegreeToRadian);
}

void SpinAttack::ApplyOffset(const Vector3 &from, const Vector3 &to, float rate) {
    // Forkはプレイヤーの子なので、ローカルZ+がプレイヤーの前方になる
    weapon_->SetMotionOffset(from + (to - from) * rate);
}

void SpinAttack::ApplyOwnerYaw(float yaw) {
    // 武器はプレイヤーの子なので、本体を回すだけで武器が周りを薙ぐ軌道になる
    // 控えた向きも足す分もY軸まわりなので、掛ける順は結果に影響しない
    ownerTransform_->SetRotationQuaternion(startRotation_ * Quaternion::FromEulerAngles({0.0f, yaw, 0.0f}));
}

void SpinAttack::SetMoveSpeedScale(float scale) {
    if (move_) {
        move_->SetSpeedScale(scale);
    }
}

int32_t SpinAttack::GetSpinTurns() const {
    return (spinTurns_ > 1) ? spinTurns_ : 1;
}

float SpinAttack::CalcPhaseRate(float phaseFrame) const {
    // 0フレーム指定でも即座に完了するようにする
    if (phaseFrame <= 0.0f) {
        return 1.0f;
    }
    return std::min(timer_ / phaseFrame, 1.0f);
}
