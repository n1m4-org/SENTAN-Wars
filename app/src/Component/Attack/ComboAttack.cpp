#include "ComboAttack.h"
#include "Character/Player/Sentan/Fork.h"
#include "Component/Attack/AttackStateComponent.h"
#include "Easing.h"
#include "Input.h"
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace Hagine;

namespace {
// コンボの入力キー（押しっぱなしで次の段へ繋がる）
constexpr BYTE kComboKey = DIK_E;
// 度からラジアンへの変換
constexpr float kDegreeToRadian = std::numbers::pi_v<float> / 180.0f;
// 構え位置（モーションによるズレが無い状態）
const Vector3 kBaseOffset{0.0f, 0.0f, 0.0f};
// 構えの角度（度）
const Vector3 kBaseAngles{0.0f, 0.0f, 0.0f};
// 1回転
constexpr float kFullTurn = std::numbers::pi_v<float> * 2.0f;

// 振りかぶり：一度溜めてから振る
constexpr EasingType kWindupEasing = EasingType::OutQuad;
// 振り抜き：初速が速く、振り切った所で止まる
constexpr EasingType kSwingEasing = EasingType::OutExpo;
// 戻し：ゆっくり構えへ戻る
constexpr EasingType kReturnEasing = EasingType::InOutQuad;
} // namespace

void ComboAttack::Update() {
    // 必須依存が無ければ何もしない
    if (!weapon_ || !attackState_) {
        return;
    }

    // 待機中は入力を待つ（他の攻撃が出ている間は攻撃できない）
    if (phase_ == Phase::Idle) {
        if (Input::GetInstance()->PushKey(kComboKey) && attackState_->CanAttack()) {
            StartAttack();
        }
        return;
    }

    PollNextInput();
    UpdateMotion();
}

void ComboAttack::StartAttack() {
    // 他の攻撃が割り込めないようにする
    attackState_->BeginAttack();
    StartStage(0);
}

void ComboAttack::StartStage(int32_t stage) {
    // 2段目以降は前の段の振り抜いた姿勢から続ける（構えを経由しないので繋がって見える）
    if (stage > 0) {
        const StageMotion previous = GetStageMotion(stage - 1);
        fromAngles_ = previous.swingAngles;
        fromOffset_ = previous.swingOffset;
        // 回しきった分は1回転単位で捨てる（同じ向きなので、巻き戻して見えない）
        fromSpin_ = std::fmod(previous.spinTurns * kFullTurn, kFullTurn);
    } else {
        fromAngles_ = kBaseAngles;
        fromOffset_ = kBaseOffset;
        fromSpin_ = 0.0f;
    }

    stage_ = stage;
    phase_ = Phase::Windup;
    timer_ = 0.0f;

    // 前の段で拾った入力は使い切ったので捨てる
    isNextRequested_ = false;
}

void ComboAttack::StartReturn() {
    // 振り抜いた姿勢から戻すため、そこを戻しの開始点として控える
    const StageMotion motion = GetStageMotion(stage_);
    fromAngles_ = motion.swingAngles;
    fromOffset_ = motion.swingOffset;
    fromSpin_ = std::fmod(motion.spinTurns * kFullTurn, kFullTurn);
    phase_ = Phase::Return;
    timer_ = 0.0f;
}

void ComboAttack::PollNextInput() {
    // 押しっぱなしで繋がる。受付が開く前（振り抜き中）に押された分も次段へ繋げる
    if (Input::GetInstance()->PushKey(kComboKey)) {
        isNextRequested_ = true;
    }
}

void ComboAttack::UpdateMotion() {
    timer_ += 1.0f;

    switch (phase_) {
    case Phase::Windup: {
        // 直前の姿勢から振りかぶった姿勢まで持っていく
        const StageMotion motion = GetStageMotion(stage_);
        const float rate = CalcPhaseRate(motion.windupFrame);
        const float easedRate = ApplyEasing<float>(kWindupEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(fromAngles_, motion.windupAngles, easedRate);
        ApplyOffset(fromOffset_, kBaseOffset, easedRate);
        ApplySpin(fromSpin_, 0.0f, easedRate);
        if (rate >= 1.0f) {
            phase_ = Phase::Swing;
            timer_ = 0.0f;

            // ここから振り抜くので、当たり判定を開ける（段ごとに開け閉めする）
            attackState_->BeginHit();
        }
        break;
    }
    case Phase::Swing: {
        // 振りかぶった姿勢から一気に振り抜く（段によっては回しながら斬る）
        const StageMotion motion = GetStageMotion(stage_);
        const float rate = CalcPhaseRate(motion.swingFrame);
        const float easedRate = ApplyEasing<float>(kSwingEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(motion.windupAngles, motion.swingAngles, easedRate);
        ApplyOffset(kBaseOffset, motion.swingOffset, easedRate);
        ApplySpin(0.0f, motion.spinTurns * kFullTurn, easedRate);
        if (rate >= 1.0f) {
            phase_ = Phase::Hold;
            timer_ = 0.0f;

            // 振り切って止まるので、当たり判定を閉じる
            // 次の段は改めて開け直すため、段の切れ目で判定も切れる
            attackState_->EndHit();
        }
        break;
    }
    case Phase::Hold: {
        // 振り抜いた所で一瞬止めることで、1振りずつが分かれて見える
        // 押しっぱなしでも必ずここを通るので、段と段の間に必ず「間」ができる
        if (timer_ >= GetStageMotion(stage_).holdFrame) {
            // 最終段まで振ったらもう繋がらないので、そのまま構えへ戻る
            if (stage_ + 1 >= kStageCount) {
                StartReturn();
            } else {
                phase_ = Phase::Window;
                timer_ = 0.0f;
            }
        }
        break;
    }
    case Phase::Window: {
        // 振り抜いた姿勢で止めたまま、次段の入力を待つ
        if (isNextRequested_) {
            StartStage(stage_ + 1);
            break;
        }
        // 受付が終わったら繋がらなかったということなので構えへ戻る
        if (timer_ >= windowFrame_) {
            StartReturn();
        }
        break;
    }
    case Phase::Return: {
        // 振り抜いた姿勢から構えへ戻す
        const float rate = CalcPhaseRate(returnFrame_);
        const float easedRate = ApplyEasing<float>(kReturnEasing, 0.0f, 1.0f, rate, 1.0f);
        ApplyRotation(fromAngles_, kBaseAngles, easedRate);
        ApplyOffset(fromOffset_, kBaseOffset, easedRate);
        ApplySpin(fromSpin_, 0.0f, easedRate);
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
            stage_ = 0;
            isNextRequested_ = false;
            attackState_->EndAttack();
        }
        break;
    }
    default:
        break;
    }
}

void ComboAttack::ApplyRotation(const Vector3 &fromDegrees, const Vector3 &toDegrees, float rate) {
    const Vector3 degrees = fromDegrees + (toDegrees - fromDegrees) * rate;
    weapon_->SetMotionRotation(degrees * kDegreeToRadian);
}

void ComboAttack::ApplyOffset(const Vector3 &from, const Vector3 &to, float rate) {
    // Forkはプレイヤーの子なので、ローカルZ+がプレイヤーの前方になる
    weapon_->SetMotionOffset(from + (to - from) * rate);
}

void ComboAttack::ApplySpin(float from, float to, float rate) {
    // 武器自身の軸まわりの回転（斬りの向きとは別に回す）
    weapon_->SetMotionSpin(from + (to - from) * rate);
}

ComboAttack::StageMotion ComboAttack::GetStageMotion(int32_t stage) const {
    switch (stage) {
    case 0:
        return {windupAngles1_, swingAngles1_, swingOffset1_, spinTurns1_,
                windupFrame1_,  swingFrame1_,  holdFrame1_};
    case 1:
        return {windupAngles2_, swingAngles2_, swingOffset2_, spinTurns2_,
                windupFrame2_,  swingFrame2_,  holdFrame2_};
    default:
        return {windupAngles3_, swingAngles3_, swingOffset3_, spinTurns3_,
                windupFrame3_,  swingFrame3_,  holdFrame3_};
    }
}

float ComboAttack::CalcPhaseRate(float phaseFrame) const {
    // 0フレーム指定でも即座に完了するようにする
    if (phaseFrame <= 0.0f) {
        return 1.0f;
    }
    return std::min(timer_ / phaseFrame, 1.0f);
}
