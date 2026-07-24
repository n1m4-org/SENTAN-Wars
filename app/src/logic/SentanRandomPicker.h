#pragma once
#include <Character/Player/Sentan/SentanDefinition.h>

inline SentanId PickRandomSentanId()
{
    // SentanIdの範囲を取得
    int minId = static_cast<int>(SentanId::Sentan1);
    int maxId = static_cast<int>(SentanId::Sentan5);
    // ランダムな整数を生成
    int randomId = minId + (std::rand() % (maxId - minId + 1));
    return static_cast<SentanId>(randomId);
}