#include "SlashFlurryAttack.h"
#include "Character/Player/Sentan/Fork.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Easing.h"
#include "Input.h"
#include <algorithm>
#include <numbers>

using namespace Hagine;

namespace {
// 連続斬撃の入力キー
constexpr BYTE kSlashKey = DIK_Q;
// 度からラジアンへの変換
constexpr float kDegreeToRadian = std::numbers::pi_v<float> / 180.0f;
// 構え位置（モーションによるズレが無い状態）
const Vector3 kBaseOffset{0.0f, 0.0f, 0.0f};
// 構えの角度（度）
const Vector3 kBaseAngles{0.0f, 0.0f, 0.0f};

// 構えへ入る：素直に持っていく
constexpr EasingType kWindupEasing = EasingType::OutQuad;
// 斬り：初速が速く、振り切った所で止まる
constexpr EasingType kSwingEasing = EasingType::OutExpo;
// 返し：次の振りの始点へ滑らかに繋ぐ
constexpr EasingType kGapEasing = EasingType::InOutQuad;
// 戻し：ゆっくり構えへ戻る
constexpr EasingType kReturnEasing = EasingType::InOutQuad;
} // namespace

void SlashFlurryAttack::Update() {
    // 必須依存が無ければ何もしない
    if (!weapon_ || !attackState_) {
        return;
    }

    // 待機中は入力を待つ（他の攻撃が出ている間は攻撃できない）
    if (phase_ == Phase::Idle) {
        if (Input::GetInstance()->TriggerKey(kSlashKey) && attackState_->CanAttack()) {
            StartAttack();
        }
        return;
    }

    UpdateMotion();
}

void SlashFlurryAttack::StartAttack() {
    // 他の攻撃が割り込めないようにする
    attackState_->BeginAttack();

    phase_ = Phase::Windup;
    swingIndex_ = 0;
    timer_ = 0.0f;

    // 他の攻撃が回した自転が残っていても、斬撃は自転させないので戻しておく
    weapon_->SetMotionSpin(0.0f);
}

void SlashFlurryAttack::StartReturn() {
    phase_ = Phase::Return;
    timer_ = 0.0f;
}

void SlashFlurryAttack::UpdateMotion() {
    timer_ += 1.0f;

    switch (phase_) {
    case Phase::Windup: {
        // 構えから1振り目の始点へ持っていく
        const float rate = CalcPhaseRate(windupFrame_);
        const float easedRate = ApplyEasing<float>(kWindupEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(kBaseAngles, CalcSwingStartAngles(0), easedRate);
        ApplyOffset(kBaseOffset, kBaseOffset, easedRate);
        if (rate >= 1.0f) {
            phase_ = Phase::Swing;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Swing: {
        // 始点から終点へ一気に斬り抜く
        const float rate = CalcPhaseRate(swingFrame_);
        const float easedRate = ApplyEasing<float>(kSwingEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(CalcSwingStartAngles(swingIndex_), CalcSwingEndAngles(swingIndex_), easedRate);
        ApplyOffset(kBaseOffset, swingOffset_, easedRate);
        if (rate >= 1.0f) {
            // 最後まで振ったら斬り終わりを見せる
            phase_ = (swingIndex_ + 1 >= GetSwingCount()) ? Phase::Finish : Phase::Gap;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Gap: {
        // 斬り終わりから次の振りの始点へ返す
        // ここが1振りずつの区切りになる（止めるのではなく返すので、軌道が飛ばない）
        const float rate = CalcPhaseRate(gapFrame_);
        const float easedRate = ApplyEasing<float>(kGapEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(CalcSwingEndAngles(swingIndex_), CalcSwingStartAngles(swingIndex_ + 1), easedRate);
        ApplyOffset(swingOffset_, kBaseOffset, easedRate);
        if (rate >= 1.0f) {
            ++swingIndex_;
            phase_ = Phase::Swing;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Finish: {
        // 振り切った姿勢で止めて、斬り終わりを見せる
        if (timer_ >= finishFrame_) {
            StartReturn();
        }
        break;
    }
    case Phase::Return: {
        // 最後に振り抜いた姿勢から構えへ戻す
        const float rate = CalcPhaseRate(returnFrame_);
        const float easedRate = ApplyEasing<float>(kReturnEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(CalcSwingEndAngles(swingIndex_), kBaseAngles, easedRate);
        ApplyOffset(swingOffset_, kBaseOffset, easedRate);
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
            swingIndex_ = 0;
            attackState_->EndAttack();
        }
        break;
    }
    default:
        break;
    }
}

void SlashFlurryAttack::ApplyRotation(const Vector3 &fromDegrees, const Vector3 &toDegrees, float rate) {
    const Vector3 degrees = fromDegrees + (toDegrees - fromDegrees) * rate;
    weapon_->SetMotionRotation(degrees * kDegreeToRadian);
}

void SlashFlurryAttack::ApplyOffset(const Vector3 &from, const Vector3 &to, float rate) {
    // Forkはプレイヤーの子なので、ローカルZ+がプレイヤーの前方になる
    weapon_->SetMotionOffset(from + (to - from) * rate);
}

Vector3 SlashFlurryAttack::CalcSwingStartAngles(int32_t index) const {
    // 1振りごとに始点と終点が入れ替わるので、往復しながら斬る
    const Vector3 &base = (index % 2 == 0) ? slashFromAngles_ : slashToAngles_;
    return base + planeStepAngles_ * static_cast<float>(index);
}

Vector3 SlashFlurryAttack::CalcSwingEndAngles(int32_t index) const {
    const Vector3 &base = (index % 2 == 0) ? slashToAngles_ : slashFromAngles_;
    return base + planeStepAngles_ * static_cast<float>(index);
}

int32_t SlashFlurryAttack::GetSwingCount() const {
    return (swingCount_ > 1) ? swingCount_ : 1;
}

float SlashFlurryAttack::CalcPhaseRate(float phaseFrame) const {
    // 0フレーム指定でも即座に完了するようにする
    if (phaseFrame <= 0.0f) {
        return 1.0f;
    }
    return std::min(timer_ / phaseFrame, 1.0f);
}
