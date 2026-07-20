#include "SentanDefinition.h"
#include "Component/Attack/AttackContext.h"

namespace {

/// SENTANの定義テーブル
/// ここが「どのSENTANでどの攻撃が使えるか」の置き場所
/// createAttack が nullptr のSENTANは、まだ攻撃が実装されていないもの
constexpr SentanDefinition kSentanDefinitions[] = {
    // id                 modelPath  createAttack
    {SentanId::Sentan1,   nullptr,   nullptr}, // 溜め攻撃（未実装）
    {SentanId::Sentan2,   nullptr,   nullptr}, // 突進（未実装）
    {SentanId::Sentan3,   nullptr,   nullptr}, // 吸い込み（未実装）
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
