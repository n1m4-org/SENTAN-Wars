#pragma once
#include "Component/Component.h"
#include <functional>
#include <utility>
#include <vector>

/// 判定を開けている間の中身
/// 攻撃ごと（同じ攻撃でも段ごと）に変えられる
struct HitWindow {
    float damage = 0.0f;    // 当てたときの攻撃力
    float sizeScale = 1.0f; // 武器の当たり判定を何倍に広げるか（1.0で武器の形そのまま）
};

/// 攻撃系コンポーネントが共有する状態
/// 攻撃コンポーネント同士は互いを知らないので、「今どれか攻撃中か」はここに集める
///
/// 「攻撃中」と「当たり判定を開けている間」は別物として扱う
/// 攻撃中はモーション全体（振りかぶり・戻し・硬直を含む）で、他の攻撃を割り込ませないための区間
/// 判定を開けている間は刃が実際に動いている区間だけで、こちらが当たり判定の有効区間になる
class AttackStateComponent : public Component {
  public:
    /// 当たり判定の開閉を受け取るコールバック
    /// isOpen が true で開く。hit はその一撃の中身（閉じるときは既定値）
    using HitWindowCallback = std::function<void(bool isOpen, const HitWindow &hit)>;

    /// 新しく攻撃を始められるか（他の攻撃が出ていないか）
    bool CanAttack() const { return !isAttacking_; }

    /// どれかの攻撃が出ている最中か
    bool IsAttacking() const { return isAttacking_; }

    /// 攻撃の開始を通知する
    void BeginAttack() { isAttacking_ = true; }

    /// 攻撃の終了を通知する
    /// 判定を閉じ忘れたまま終わっても開きっぱなしにならないよう、ここで必ず閉じる
    void EndAttack() {
        EndHit();
        isAttacking_ = false;
    }

    /// 当たり判定を開けている最中か
    bool IsHitting() const { return isHitting_; }

    /// 当たり判定を開ける（攻撃コンポーネントが刃を振り始めるときに呼ぶ）
    void BeginHit(const HitWindow &hit) {
        if (isHitting_) {
            return;
        }
        isHitting_ = true;
        hit_ = hit;
        NotifyHitWindow(true, hit_);
    }

    /// 当たり判定を閉じる（攻撃コンポーネントが刃を振り終えたときに呼ぶ）
    void EndHit() {
        if (!isHitting_) {
            return;
        }
        isHitting_ = false;
        hit_ = {};
        NotifyHitWindow(false, hit_);
    }

    /// 今開けている判定の中身（閉じているときは既定値）
    const HitWindow &GetHitWindow() const { return hit_; }

    /// 判定の開閉を知りたい相手を追加する
    /// 攻撃側は誰が聞いているかを知らないままでいられる
    void AddHitWindowCallback(HitWindowCallback callback) {
        if (callback) {
            hitWindowCallbacks_.emplace_back(std::move(callback));
        }
    }

  private:
    /// 判定の開閉を知りたい相手へ配る
    void NotifyHitWindow(bool isOpen, const HitWindow &hit) {
        for (const HitWindowCallback &callback : hitWindowCallbacks_) {
            callback(isOpen, hit);
        }
    }

  private:
    // どれかの攻撃が出ている最中か
    bool isAttacking_ = false;

    // 当たり判定を開けている最中か
    bool isHitting_ = false;

    // 今開けている判定の中身
    HitWindow hit_{};

    // 判定の開閉を知りたい相手
    std::vector<HitWindowCallback> hitWindowCallbacks_;
};
