#pragma once
#include "type/Vector3.h"
#include <memory>

class Component;
struct SentanContext;

/// 準備フェーズで取得できるSENTANの種類
enum class SentanId {
    Sentan1, // 突進
    Sentan2, // 二段ジャンプ
    Sentan3, // 3段コンボ
    Sentan4, // 連続斬撃
};

/// SENTANが解禁するコンポーネントの生成関数
using CreateComponentFunc = std::unique_ptr<Component> (*)(const SentanContext &);

/// SENTAN1種類分の定義
/// SENTANを増やすときは「コンポーネントを1つ作る」＋「定義テーブルに1行足す」だけでよい
/// Player も Fork も、解禁される振る舞いの種類を一切知らないままでいられる
struct SentanDefinition {
    SentanId id;                         // 種類
    const char *modelPath;               // Forkにくっつけるモデル
    CreateComponentFunc createComponent; // このSENTANを装備したときだけ増える振る舞い
    // 当たり判定の大きさ（中心からの半径）
    // モデルがSENTANごとに違うので、形に合わせてここで書き換える
    Hagine::Vector3 colliderSize;
};

/// 定義テーブルからSENTANの定義を引く
const SentanDefinition *FindSentanDefinition(SentanId id);
