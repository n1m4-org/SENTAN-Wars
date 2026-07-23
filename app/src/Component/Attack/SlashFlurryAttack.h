#pragma once
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"
#include <cstdint>

class Fork;
class AttackStateComponent;

/// 連続斬撃（SENTANで解禁される攻撃）
/// 1回の入力で武器を6回ほど振る。振りごとに手で角度を決めるのではなく、
/// 「2つの角度を往復しながら、1振りごとに斬る面をずらす」規則で軌道を作る
/// （振る回数を変えてもパラメータが増えないので、回数はそのまま調整できる）
class SlashFlurryAttack : public Component {
  public:
    /// 攻撃に必要な物はまとめて受け取る
    explicit SlashFlurryAttack(const SentanContext &context)
        : weapon_(context.weapon), attackState_(context.attackState) {}

    void Update() override;

  private:
    /// 攻撃の進行フェーズ
    enum class Phase {
        Idle,     // 待機（入力待ち）
        Windup,   // 1振り目の構えへ入る
        Swing,    // 斬る
        Gap,      // 次の振りの始点へ返す（振りと振りの間）
        Finish,   // 斬り終わりを見せる
        Return,   // 構えへ戻す
        CoolTime, // 攻撃後の硬直
    };

    /// 攻撃を開始する
    void StartAttack();

    /// 構えへ戻し始める
    void StartReturn();

    /// 攻撃モーションを進める
    void UpdateMotion();

    /// 武器の回転を2つの角度の間で補間する（角度は度）
    void ApplyRotation(const Hagine::Vector3 &fromDegrees, const Hagine::Vector3 &toDegrees, float rate);

    /// 武器の位置を2点間で補間する（rateは0.0でfrom、1.0でto）
    void ApplyOffset(const Hagine::Vector3 &from, const Hagine::Vector3 &to, float rate);

    /// 指定の振りの始点の角度（度）
    Hagine::Vector3 CalcSwingStartAngles(int32_t index) const;

    /// 指定の振りの終点の角度（度）
    Hagine::Vector3 CalcSwingEndAngles(int32_t index) const;

    /// 実際に振る回数（0以下を指定されても1回は振る）
    int32_t GetSwingCount() const;

    /// フェーズの経過フレームから進行度(0.0〜1.0)を求める
    float CalcPhaseRate(float phaseFrame) const;

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("SlashFlurryAttack");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Fork *weapon_ = nullptr;                      // 振る武器
    AttackStateComponent *attackState_ = nullptr; // 攻撃中フラグの共有先

    // ==== 状態 ====
    Phase phase_ = Phase::Idle; // 現在のフェーズ
    int32_t swingIndex_ = 0;    // 何振り目か（0始まり）
    float timer_ = 0.0f;        // 現在フェーズの経過フレーム

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    GameParameter(float, atk_, 5.0f); // 1振りあたりの攻撃力（当たり判定ができたらダメージに使う）
    GameParameter(int, swingCount_, 6); // 振る回数

    // 斬りの軌道：この2つの角度（度）の間を1振りごとに往復する
    GameParameter(Hagine::Vector3, slashFromAngles_, (Hagine::Vector3{-60.0f, 55.0f, -55.0f}));
    GameParameter(Hagine::Vector3, slashToAngles_, (Hagine::Vector3{-120.0f, -55.0f, 55.0f}));
    // 1振りごとに斬る面をずらす量（度）。同じ軌道の往復に見えないようにする
    GameParameter(Hagine::Vector3, planeStepAngles_, (Hagine::Vector3{0.0f, 0.0f, 30.0f}));
    // 振り抜いたときの構えからのズレ（1振りごとに前へ出て戻る）
    GameParameter(Hagine::Vector3, swingOffset_, (Hagine::Vector3{0.0f, 0.0f, 0.6f}));

    GameParameter(float, windupFrame_, 6.0f);    // 1振り目の構えへ入るフレーム数
    GameParameter(float, swingFrame_, 5.0f);     // 1振りにかけるフレーム数（短いほど鋭い）
    GameParameter(float, gapFrame_, 3.0f);       // 振りと振りの間のフレーム数
    GameParameter(float, finishFrame_, 14.0f);   // 斬り終わりを見せるフレーム数
    GameParameter(float, returnFrame_, 12.0f);   // 構えへ戻すフレーム数
    GameParameter(float, coolTimeFrame_, 16.0f); // 攻撃後の硬直フレーム数
};
