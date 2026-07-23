#include "SentanDefinition.h"
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Attack/ComboAttack.h"
#include "Component/Attack/DrillRushAttack.h"
#include "Component/Attack/SlashFlurryAttack.h"
#include "Component/Attack/SpinAttack.h"
#include "Component/DoubleJumpComponent.h"

namespace {

/// SENTANの定義テーブル
/// ここが「どのSENTANで何が解禁されるか」の置き場所
/// createComponent が nullptr のSENTANは、まだ振る舞いが実装されていないもの
///
/// colliderSize は当たり判定の大きさ（中心からの半径）
/// コライダーはスケールを見ないため、モデルの実寸ではなくワールドでの大きさで書く
/// 今はどれも仮のCube（2×2×2）で、Forkが 0.3 倍で装着するので 0.3 が実寸ぴったりになる
/// モデルを差し替えたら、その形に合わせてここだけ書き換える
constexpr SentanDefinition kSentanDefinitions[] = {
    // id                 modelPath  createComponent                                   colliderSize
    {SentanId::Sentan1, nullptr, &CreateSentanComponent<DrillRushAttack>, {0.3f, 0.3f, 0.3f}},     // 突進
    {SentanId::Sentan2, nullptr, &CreateSentanComponent<DoubleJumpComponent>, {0.3f, 0.3f, 0.3f}}, // 二段ジャンプ
    {SentanId::Sentan3, nullptr, &CreateSentanComponent<ComboAttack>, {0.3f, 0.3f, 0.3f}},         // 3段コンボ
    {SentanId::Sentan4, nullptr, &CreateSentanComponent<SlashFlurryAttack>, {0.3f, 0.3f, 0.3f}},   // 連続斬撃
    {SentanId::Sentan5, nullptr, &CreateSentanComponent<SpinAttack>, {0.3f, 0.3f, 0.3f}},          // 回転攻撃
};

} // namespace

const SentanDefinition *FindSentanDefinition(SentanId id) {
    for (const SentanDefinition &definition : kSentanDefinitions) {
        if (definition.id == id) {
            return &definition;
        }
    }
    return nullptr;
}
