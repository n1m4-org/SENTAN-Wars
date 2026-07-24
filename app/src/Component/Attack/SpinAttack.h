#pragma once
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Quaternion.h"
#include "type/Vector3.h"
#include <cstdint>

namespace Hagine {
class WorldTransform;
} // namespace Hagine

class Fork;
class AttackStateComponent;
class MoveComponent;

/// 回転攻撃（SENTANで解禁される攻撃）
/// 武器を横へ突き出したまま本体ごとその場で回り、周りをまとめて薙ぎ払う
/// 1周ごとに判定を開け直すので、回った回数だけ当たる
class SpinAttack : public Component {
  public:
    /// 攻撃に必要な物はまとめて受け取る
    explicit SpinAttack(const SentanContext &context)
        : ownerTransform_(context.ownerTransform), weapon_(context.weapon), attackState_(context.attackState),
          move_(context.move) {}

    /// 回転を止め忘れたまま消えても、移動の速度を戻しておく
    ~SpinAttack() override;

    void Update() override;

  private:
    /// 攻撃の進行フェーズ
    enum class Phase {
        Idle,     // 待機（入力待ち）
        Windup,   // 武器を横へ構える
        Spin,     // 本体ごと回る
        Finish,   // 回り終わりを見せる
        Return,   // 構えへ戻す
        CoolTime, // 攻撃後の硬直
    };

    /// 攻撃を開始する
    void StartAttack();

    /// 回転を開始する（回り始めの向きを控える）
    void StartSpin();

    /// 攻撃モーションを進める
    void UpdateMotion();

    /// 回転を1フレーム分進める
    void UpdateSpin();

    /// 武器の回転を2つの角度の間で補間する（角度は度）
    void ApplyRotation(const Hagine::Vector3 &fromDegrees, const Hagine::Vector3 &toDegrees, float rate);

    /// 武器の位置を2点間で補間する（rateは0.0でfrom、1.0でto）
    void ApplyOffset(const Hagine::Vector3 &from, const Hagine::Vector3 &to, float rate);

    /// 本体を回り始めの向きから指定角だけ回す（ラジアン）
    void ApplyOwnerYaw(float yaw);

    /// 回る周回数（1未満にならないようにする）
    int32_t GetSpinTurns() const;

    /// 移動の速度倍率を設定する（moveが無くても安全に呼べる）
    void SetMoveSpeedScale(float scale);

    /// フェーズの経過フレームから進行度(0.0〜1.0)を求める
    float CalcPhaseRate(float phaseFrame) const;

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("SpinAttack");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform *ownerTransform_ = nullptr; // 回す本体
    Fork *weapon_ = nullptr;                           // 横へ構える武器
    AttackStateComponent *attackState_ = nullptr;      // 攻撃中フラグの共有先
    MoveComponent *move_ = nullptr;                    // 任意：回転中に移動を鈍らせる相手

    // ==== 状態 ====
    Phase phase_ = Phase::Idle; // 現在のフェーズ
    float timer_ = 0.0f;        // 現在フェーズの経過フレーム
    int32_t hitIndex_ = 0;      // 今何周目を当てているか
    // 回り始めたときの本体の向き（ここを基準に回す）
    Hagine::Quaternion startRotation_ = Hagine::Quaternion::IdentityQuaternion();

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    // 攻撃力。周りをまとめて薙ぐぶん1発は軽い（2周で24）
    GameParameter(float, atk_, 12.0f);
    // 判定の広さ（武器の形の何倍か）。薙ぎ払いなので広め
    GameParameter(float, hitScale_, 5.5f);
    // 回転中の移動速度の倍率（1.0で普段どおり、0.0で足が止まる）
    GameParameter(float, moveSpeedScale_, 0.4f);

    // 構えたときの武器の角度（度）：X=-90で横へ倒し、回転で薙ぐ形になる
    GameParameter(Hagine::Vector3, holdAngles_, (Hagine::Vector3{-90.0f, 0.0f, 0.0f}));
    // 構えたときの構え位置からのズレ：Zで前へ突き出すほど大きく回る
    GameParameter(Hagine::Vector3, holdOffset_, (Hagine::Vector3{0.0f, 0.0f, 1.5f}));

    GameParameter(int, spinTurns_, 2);            // 回る周回数（＝当たる回数）
    GameParameter(float, windupFrame_, 12.0f);    // 横へ構えるフレーム数
    GameParameter(float, spinFrame_, 30.0f);      // 回りきるフレーム数（短いほど速い）
    GameParameter(float, finishFrame_, 10.0f);    // 回り終わりを見せるフレーム数
    GameParameter(float, returnFrame_, 12.0f);    // 構えへ戻すフレーム数
    GameParameter(float, coolTimeFrame_, 25.0f);  // 攻撃後の硬直フレーム数
};
