#pragma once
#include <memory>

class Component;
struct AttackContext;

/// 準備フェーズで取得できるSENTANの種類
enum class SentanId {
    Sentan1, // 溜め攻撃
    Sentan2, // 突進
    Sentan3, // 吸い込み
};

/// SENTANが解禁する攻撃コンポーネントの生成関数
using CreateAttackFunc = std::unique_ptr<Component> (*)(const AttackContext &);

/// SENTAN1種類分の定義
/// SENTANを増やすときは「攻撃コンポーネントを1つ作る」＋「定義テーブルに1行足す」だけでよい
/// Player も Fork も攻撃の種類を一切知らないままでいられる
struct SentanDefinition {
    SentanId id;                   // 種類
    const char *modelPath;         // Forkにくっつけるモデル
    CreateAttackFunc createAttack; // このSENTANを装備したときだけ使えるようになる攻撃
};

/// 定義テーブルからSENTANの定義を引く
const SentanDefinition *FindSentanDefinition(SentanId id);
