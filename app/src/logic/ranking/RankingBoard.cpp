#include "RankingBoard.h"
#include <algorithm>
#include <string>
#include <vector>
#include <data/DataHandler.h>

namespace
{
    // JSON の保存先（Assets/jsons/Ranking/ClearTime.json）
    constexpr const char* kFolder = "Ranking";
    constexpr const char* kFile = "ClearTime";

    // 順位ごとの JSON キー（"rank1", "rank2", ...）。件数に依らず生成する
    std::string RankKey(size_t index)
    {
        return "rank" + std::to_string(index + 1);
    }
}

void RankingBoard::Load()
{
    Hagine::DataHandler data(kFolder, kFile);

    // 有効な記録だけ集めて昇順（速い順）に整列し、上位 kEntryCount 件へ反映する
    std::vector<float> times;
    for (size_t i = 0; i < kEntryCount; ++i)
    {
        const float time = data.Load<float>(RankKey(i), kEmpty);
        if (time > 0.0f)
        {
            times.push_back(time);
        }
    }
    std::sort(times.begin(), times.end());

    entries_.fill(kEmpty);
    for (size_t i = 0; i < kEntryCount && i < times.size(); ++i)
    {
        entries_[i] = times[i];
    }
}

int RankingBoard::AddTime(float seconds)
{
    if (seconds <= 0.0f)
    {
        return -1; // 無効なタイムは登録しない
    }

    // 既存の有効な記録に今回のタイムを加えて昇順に整列する
    std::vector<float> times;
    for (const float time : entries_)
    {
        if (time > 0.0f)
        {
            times.push_back(time);
        }
    }
    times.push_back(seconds);
    std::sort(times.begin(), times.end());

    // 上位 kEntryCount 件へ反映する
    entries_.fill(kEmpty);
    for (size_t i = 0; i < kEntryCount && i < times.size(); ++i)
    {
        entries_[i] = times[i];
    }

    // 今回のタイムが入った順位を求める（同値のときは最上位の一致位置）
    int rank = -1;
    for (size_t i = 0; i < kEntryCount; ++i)
    {
        if (entries_[i] == seconds)
        {
            rank = static_cast<int>(i);
            break;
        }
    }

    this->Save();
    return rank;
}

void RankingBoard::Save() const
{
    Hagine::DataHandler data(kFolder, kFile);
    for (size_t i = 0; i < kEntryCount; ++i)
    {
        data.Save<float>(RankKey(i), entries_[i]);
    }
    data.Flush();
}
