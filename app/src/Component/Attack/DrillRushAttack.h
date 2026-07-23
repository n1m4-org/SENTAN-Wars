#pragma once
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"

namespace Hagine {
class WorldTransform;
} // namespace Hagine

class Fork;
class AttackStateComponent;

/// 突進（SENTANで解禁される攻撃）
/// 武器(Fork)を前方へ倒しながらドリルのように自転させ、
/// 本体を向いている方向へ素早く滑らせる
class DrillRushAttack : public Component {
  public:
    /// 攻撃に必要な物はまとめて受け取る
    explicit DrillRushAttack(const SentanContext &context)
        : ownerTransform_(context.ownerTransform), weapon_(context.weapon), attackState_(context.attackState) {}

    void Update() override;

  private:
    /// 攻撃の進行フェーズ
    enum class Phase {
        Idle,     // 待機（入力待ち）
        Tilt,     // 前方へ倒しながら後方へ引く
        SpinUp,   // 引ききった位置でドリルを回し始める
        Rush,     // 回しながら前方へ突進する
        Return,   // 構えへ戻す
        CoolTime, // 攻撃後の硬直
    };

    /// 攻撃を開始する
    void StartAttack();

    /// 突進を開始する（開始位置と進む向きを決める）
    void StartRush();

    /// 攻撃モーションを進める
    void UpdateMotion();

    /// 武器を前方へ倒す（rateは0.0で構えの向き、1.0で倒しきった向き）
    void ApplyTilt(float rate);

    /// 武器の位置を2点間で補間する（rateは0.0でfrom、1.0でto）
    void ApplyOffset(const Hagine::Vector3 &from, const Hagine::Vector3 &to, float rate);

    /// 自転を1フレーム分進める
    void AdvanceSpin();

    /// 本体を突進の開始位置から進める（rateは0.0で開始位置、1.0で到達位置）
    void ApplyRush(float rate);

    /// 本体が向いている方向（水平）を求める
    Hagine::Vector3 CalcForward() const;

    /// フェーズの経過フレームから進行度(0.0〜1.0)を求める
    float CalcPhaseRate(float phaseFrame) const;

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("DrillRushAttack");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Hagine::WorldTransform *ownerTransform_ = nullptr; // 突進させる本体
    Fork *weapon_ = nullptr;                           // 回す武器
    AttackStateComponent *attackState_ = nullptr;      // 攻撃中フラグの共有先

    // ==== 状態 ====
    Phase phase_ = Phase::Idle; // 現在のフェーズ
    float timer_ = 0.0f;        // 現在フェーズの経過フレーム
    float spinAngle_ = 0.0f;    // 武器の自転角（ラジアン）
    float spinStartAngle_ = 0.0f; // 戻し開始時の自転角
    float spinEndAngle_ = 0.0f;   // 戻しきったときの自転角（回り切った位置）
    Hagine::Vector3 rushStartPosition_{0.0f, 0.0f, 0.0f}; // 突進の開始位置
    Hagine::Vector3 rushDirection_{0.0f, 0.0f, 1.0f};     // 突進する向き（開始時に固定）

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    // 攻撃力。1発しか当たらない代わりに重い（Normal敵20を一撃、Tank50を2発）
    GameParameter(float, atk_, 25.0f);
    // 判定の広さ（武器の形の何倍か）。回しながら突っ込むので一番広い
    GameParameter(float, hitScale_, 6.0f);
    // 倒しきったときの各軸の回転角（度）：X=前方へ倒す
    GameParameter(Hagine::Vector3, tiltAngles_, (Hagine::Vector3{-90.0f, 0.0f, 0.0f}));
    // 引ききったときの構えからのズレ：Z=マイナスで後方へ引く
    GameParameter(Hagine::Vector3, pullOffset_, (Hagine::Vector3{0.0f, 0.0f, -1.5f}));
    GameParameter(float, tiltFrame_, 10.0f);   // 倒しながら引ききるまでのフレーム数
    GameParameter(float, spinUpFrame_, 30.0f); // 突進に入る前の溜め（狙いを定める間・回し始めながら待つ）
    GameParameter(float, spinSpeed_, 45.0f);   // 自転の速さ（1フレームあたりの度数）
    // 突進中に武器を突き出す量：Z=前方へ突き出す
    GameParameter(Hagine::Vector3, thrustOffset_, (Hagine::Vector3{0.0f, 0.0f, 2.5f}));
    GameParameter(float, rushDistance_, 8.0f);   // 突進で進む距離
    GameParameter(float, rushFrame_, 14.0f);     // 突進にかけるフレーム数（短いほど速い）
    GameParameter(float, returnFrame_, 10.0f);   // 構えへ戻すフレーム数
    GameParameter(float, coolTimeFrame_, 20.0f); // 攻撃後の硬直フレーム数
};
