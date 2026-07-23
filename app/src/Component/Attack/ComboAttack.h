#pragma once
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Component.h"
#include "debug/GameParameter.h"
#include "type/Vector3.h"
#include <cstdint>

class Fork;
class AttackStateComponent;

/// 3段コンボ攻撃（SENTANで解禁される攻撃）
/// 1段ごとに「振りかぶり→振り抜き→次段の受付」を行い、
/// 受付中も押されていれば次の段へ繋がる。離していれば構えへ戻る
/// （押しっぱなしで3段まで繋がり、途中で離せばそこで止まる）
class ComboAttack : public Component {
  public:
    /// 攻撃に必要な物はまとめて受け取る
    explicit ComboAttack(const SentanContext &context)
        : weapon_(context.weapon), attackState_(context.attackState) {}

    void Update() override;

  private:
    /// 攻撃の進行フェーズ
    enum class Phase {
        Idle,     // 待機（入力待ち）
        Windup,   // 振りかぶる
        Swing,    // 振り抜く
        Hold,     // 振り抜いた姿勢で止めて、1振りを見せる
        Window,   // 次段の入力を受け付ける
        Return,   // 構えへ戻す
        CoolTime, // 攻撃後の硬直
    };

    /// コンボの段数
    static constexpr int32_t kStageCount = 3;

    /// 1段分の振り方
    /// 段ごとに違う斬り方をさせるため、パラメータをまとめて引けるようにする
    struct StageMotion {
        const Hagine::Vector3 &windupAngles; // 振りかぶりきった角度（度）
        const Hagine::Vector3 &swingAngles;  // 振り抜いた角度（度）
        const Hagine::Vector3 &swingOffset;  // 振り抜いたときの構えからのズレ
        float spinTurns;                     // 振り抜く間に武器を自転させる回数
        float windupFrame;                   // 振りかぶるフレーム数
        float swingFrame;                    // 振り抜くフレーム数
        float holdFrame;                     // 振り抜いた姿勢で止めるフレーム数
        float damage;                        // その段の攻撃力
    };

    /// コンボを開始する
    void StartAttack();

    /// 指定の段を始める
    void StartStage(int32_t stage);

    /// 構えへ戻し始める（戻す元の姿勢を覚えておく）
    void StartReturn();

    /// 攻撃モーションを進める
    void UpdateMotion();

    /// 次段へ繋ぐ入力を拾う（受付前に押された分も覚えておく）
    void PollNextInput();

    /// 武器の回転を2つの角度の間で補間する（角度は度）
    void ApplyRotation(const Hagine::Vector3 &fromDegrees, const Hagine::Vector3 &toDegrees, float rate);

    /// 武器の位置を2点間で補間する（rateは0.0でfrom、1.0でto）
    void ApplyOffset(const Hagine::Vector3 &from, const Hagine::Vector3 &to, float rate);

    /// 武器の自転を2つの角度の間で補間する（角度はラジアン）
    void ApplySpin(float from, float to, float rate);

    /// 段ごとの振り方を引く
    StageMotion GetStageMotion(int32_t stage) const;

    /// フェーズの経過フレームから進行度(0.0〜1.0)を求める
    float CalcPhaseRate(float phaseFrame) const;

  private:
    // GameParameterの登録先となるデバッグ
    EnableDebug("ComboAttack");

    // ==== 挿入された依存（所有はしない・参照するだけ） ====
    Fork *weapon_ = nullptr;                      // 振る武器
    AttackStateComponent *attackState_ = nullptr; // 攻撃中フラグの共有先

    // ==== 状態 ====
    Phase phase_ = Phase::Idle;   // 現在のフェーズ
    int32_t stage_ = 0;           // 現在の段（0始まり）
    float timer_ = 0.0f;          // 現在フェーズの経過フレーム
    bool isNextRequested_ = false; // 次段へ繋ぐ入力があったか
    // 補間の開始姿勢（段の始まり・戻しの始まりで、そのときの姿勢を控える）
    Hagine::Vector3 fromAngles_{0.0f, 0.0f, 0.0f}; // 角度（度）
    Hagine::Vector3 fromOffset_{0.0f, 0.0f, 0.0f}; // 位置ズレ
    float fromSpin_ = 0.0f;                        // 自転角（ラジアン）

    // ==== 調整用パラメータ（GameParameterでデバッグ調整） ====
    // 攻撃力は段ごとに持つ（繋げるほど重くなるようにしてある）
    GameParameter(float, atk1_, 8.0f);
    GameParameter(float, atk2_, 10.0f);
    GameParameter(float, atk3_, 18.0f); // 締めなので一番重い
    // 判定の広さ（武器の形の何倍か）。薙ぐように斬るので広め
    GameParameter(float, hitScale_, 5.5f);

    // 1段目：袈裟斬り（右上から左下へ斜めに斬り下ろす）
    GameParameter(Hagine::Vector3, windupAngles1_, (Hagine::Vector3{-55.0f, 50.0f, -55.0f}));
    GameParameter(Hagine::Vector3, swingAngles1_, (Hagine::Vector3{-115.0f, -45.0f, 45.0f}));
    GameParameter(Hagine::Vector3, swingOffset1_, (Hagine::Vector3{0.0f, 0.0f, 0.8f}));
    GameParameter(float, spinTurns1_, 0.0f);   // 自転させない（斬る向きを見せる）
    GameParameter(float, windupFrame1_, 6.0f); // 素早く入る
    GameParameter(float, swingFrame1_, 6.0f);
    GameParameter(float, holdFrame1_, 6.0f); // 斬った所で一瞬止める

    // 2段目：逆袈裟（左下から右上へ斜めに斬り上げる）
    GameParameter(Hagine::Vector3, windupAngles2_, (Hagine::Vector3{-125.0f, -50.0f, 50.0f}));
    GameParameter(Hagine::Vector3, swingAngles2_, (Hagine::Vector3{-55.0f, 45.0f, -45.0f}));
    GameParameter(Hagine::Vector3, swingOffset2_, (Hagine::Vector3{0.0f, 0.0f, 0.8f}));
    GameParameter(float, spinTurns2_, 0.0f);
    GameParameter(float, windupFrame2_, 5.0f); // 返しなので更に速く
    GameParameter(float, swingFrame2_, 6.0f);
    GameParameter(float, holdFrame2_, 6.0f);

    // 3段目：大きく振り上げ、回しながら振り下ろす（締め）
    GameParameter(Hagine::Vector3, windupAngles3_, (Hagine::Vector3{-160.0f, 0.0f, 0.0f}));
    GameParameter(Hagine::Vector3, swingAngles3_, (Hagine::Vector3{-30.0f, 0.0f, 0.0f}));
    GameParameter(Hagine::Vector3, swingOffset3_, (Hagine::Vector3{0.0f, 0.0f, 2.0f}));
    GameParameter(float, spinTurns3_, 1.0f);    // 1回転させて締める
    GameParameter(float, windupFrame3_, 12.0f); // 溜めて重くする
    GameParameter(float, swingFrame3_, 8.0f);
    GameParameter(float, holdFrame3_, 12.0f); // 締めなので長めに見せる

    GameParameter(float, windowFrame_, 16.0f);  // 次段を受け付けるフレーム数
    GameParameter(float, returnFrame_, 12.0f);  // 構えへ戻すフレーム数
    GameParameter(float, coolTimeFrame_, 8.0f); // 攻撃後の硬直フレーム数
};
