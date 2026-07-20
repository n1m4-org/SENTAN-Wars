#include "SentanDefinition.h"
#include "Character/Player/Sentan/SentanContext.h"
#include "Component/DoubleJumpComponent.h"

namespace {

/// SENTANの定義テーブル
/// ここが「どのSENTANで何が解禁されるか」の置き場所
/// createComponent が nullptr のSENTANは、まだ振る舞いが実装されていないもの
constexpr SentanDefinition kSentanDefinitions[] = {
    // id                 modelPath  createComponent
    {SentanId::Sentan1, nullptr, nullptr},                                     // 溜め攻撃（未実装）
    {SentanId::Sentan2, nullptr, &CreateSentanComponent<DoubleJumpComponent>}, // 二段ジャンプ
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
