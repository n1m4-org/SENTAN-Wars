#pragma once
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"

class Fork;
class AttackStateComponent;
class MoveComponent;

/// 通常攻撃
/// SENTANを持っていなくても常に使えるので、Playerが最初から持っている唯一の攻撃
/// 武器(Fork)を振りかぶって前方へ倒し、鋭く突き刺してから構えへ戻すモーションを行う
class NormalAttack : public Component {
  public:
    /// 攻撃に必要な物はまとめて受け取る
    explicit NormalAttack(const SentanContext &context)
        : weapon_(context.weapon), attackState_(context.attackState), move_(context.move) {}

    /// 攻撃中に消えても、移動の速度を戻しておく
    ~NormalAttack() override;

    void Update() override;

  private:
    /// 攻撃の進行フェーズ
    enum class Phase {
        Idle,     // 待機（入力待ち）
        Tilt,     // 前方へ倒しながら後方へ引く
        Charge,   // 引ききった位置で溜める
        Thrust,   // 鋭く突き刺す
        Hold,     // 突き刺したまま止める
        Return,   // 構えへ戻す
        CoolTime, // 攻撃後の硬直
    };

    /// 攻撃を開始する
    void StartAttack();

    /// 攻撃モーションを進める
    void UpdateMotion();

    /// 武器を前方へ倒す（rateは0.0で構えの向き、1.0で倒しきった向き）
    void ApplyTilt(float rate);

    /// 武器の位置を2点間で補間する（rateは0.0でfrom、1.0でto）
    void ApplyOffset(const Hagine::Vector3 &from, const Hagine::Vector3 &to, float rate);

    /// フェーズの経過フレームから進行度(0.0〜1.0)を求める
    float CalcPhaseRate(float phaseFrame) const;

    /// 移動の速度倍率を設定する（moveが無くても安全に呼べる）
    void SetMoveSpeedScale(float scale);

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("NormalAttack");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Fork *weapon_ = nullptr;                      // 振る武器
    AttackStateComponent *attackState_ = nullptr; // 攻撃中フラグの共有先
    MoveComponent *move_ = nullptr;               // 任意：攻撃中に移動を鈍らせる相手

    // ==== 状態 ====
    Phase phase_ = Phase::Idle; // 現在のフェーズ
    float timer_ = 0.0f;        // 現在フェーズの経過フレーム

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    // 攻撃力。SENTANが無くても常に使えるので控えめ（Normal敵20を2発）
    GameParameter(float, atk_, 10.0f);
    // 判定の広さ（武器の形の何倍か）。突きなので、当てやすさは控えめ
    GameParameter(float, hitScale_, 4.0f);
    // 攻撃中の移動速度の倍率（1.0で普段どおり、0.0で足が止まる）
    GameParameter(float, moveSpeedScale_, 0.4f);
    // 倒しきったときの各軸の回転角（度）：X=前方へ倒す / Z=軸まわりにひねる
    GameParameter(Hagine::Vector3, tiltAngles_, (Hagine::Vector3{-90.0f, 0.0f, 0.0f}));
    // 引ききったときの構えからのズレ：Z=マイナスで後方へ引く
    GameParameter(Hagine::Vector3, pullOffset_, (Hagine::Vector3{0.0f, 0.0f, -1.5f}));
    GameParameter(float, tiltFrame_, 10.0f);   // 倒しながら引ききるまでのフレーム数
    GameParameter(float, chargeFrame_, 15.0f); // 引ききった位置で溜めるフレーム数
    // 突き刺しきったときの構えからのズレ：Z=前方へ突き出す / Y=沈み込ませたい場合に使う
    GameParameter(Hagine::Vector3, thrustOffset_, (Hagine::Vector3{0.0f, 0.0f, 2.5f}));
    GameParameter(float, thrustFrame_, 6.0f); // 突き刺しにかけるフレーム数（短いほど鋭い）
    GameParameter(float, holdFrame_, 10.0f);       // 突き刺したまま止めるフレーム数
    GameParameter(float, returnFrame_, 10.0f);    // 構えへ戻すフレーム数
    GameParameter(float, coolTimeFrame_, 10.0f);  // 攻撃後の硬直フレーム数
};
