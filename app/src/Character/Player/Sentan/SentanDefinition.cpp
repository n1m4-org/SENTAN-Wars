#include "SentanDefinition.h"
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/Attack/ComboAttack.h"
#include "Component/Attack/DrillRushAttack.h"
#include "Component/Attack/SlashFlurryAttack.h"
#include "Component/DoubleJumpComponent.h"

namespace {

/// SENTANの定義テーブル
/// ここが「どのSENTANで何が解禁されるか」の置き場所
/// createComponent が nullptr のSENTANは、まだ振る舞いが実装されていないもの
constexpr SentanDefinition kSentanDefinitions[] = {
    // id                 modelPath  createComponent
    {SentanId::Sentan1, nullptr, &CreateSentanComponent<DrillRushAttack>},     // 突進
    {SentanId::Sentan2, nullptr, &CreateSentanComponent<DoubleJumpComponent>}, // 二段ジャンプ
    {SentanId::Sentan3, nullptr, &CreateSentanComponent<ComboAttack>},         // 3段コンボ
    {SentanId::Sentan4, nullptr, &CreateSentanComponent<SlashFlurryAttack>},   // 連続斬撃
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
