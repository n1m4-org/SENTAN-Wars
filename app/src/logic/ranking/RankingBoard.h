#pragma once
#include <array>
#include <cstddef>

/// <summary>
/// クリアタイムのランキング（上位 kEntryCount 件）を JSON で永続化するクラス。
/// タイムは秒。値が小さい（速い）ほど上位。空きスロットは kEmpty で表す。
/// 保存先は Assets/jsons/Ranking/ClearTime.json。
/// </summary>
class RankingBoard
{
public:
    /// ===================================================
    /// public constant
    /// ===================================================

    // 記録・表示する上位件数。ここを変えるだけで記録順位・表示順位の両方が変わる。
    // （RankingView・ClearScene もこの定数を参照している）
    static constexpr size_t kEntryCount = 5;
    static constexpr float kEmpty = -1.0f;     // 記録なしを表す値

    /// ===================================================
    /// public method
    /// ===================================================

    /// <summary>
    /// JSON から上位 kEntryCount 件を読み込む
    /// </summary>
    void Load();

    /// <summary>
    /// タイムを登録する。上位 kEntryCount 件に入れば JSON へ保存する。
    /// </summary>
    /// <param name="seconds">クリアタイム（秒）</param>
    /// <returns>登録された順位のインデックス(0..kEntryCount-1)。ランク外なら -1</returns>
    int AddTime(float seconds);

    /// <summary>
    /// 上位 kEntryCount 件を取得（速い順。空きスロットは kEmpty）
    /// </summary>
    /// <returns>上位 kEntryCount 件のタイム配列</returns>
    const std::array<float, kEntryCount>& GetEntries() const { return entries_; }

private:
    /// ===================================================
    /// private method
    /// ===================================================

    /// <summary>
    /// 現在の上位 kEntryCount 件を JSON へ書き出す
    /// </summary>
    void Save() const;

    /// ===================================================
    /// private variables
    /// ===================================================

    // 全スロットを kEmpty で初期化する（件数に依らず空き状態から始める）
    std::array<float, kEntryCount> entries_ = []
    {
        std::array<float, kEntryCount> a{};
        a.fill(kEmpty);
        return a;
    }();
};
