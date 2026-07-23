#pragma once
#include <array>
#include <cstddef>

/// <summary>
/// クリアタイムのランキング（上位 3 件）を JSON で永続化するクラス。
/// タイムは秒。値が小さい（速い）ほど上位。空きスロットは kEmpty で表す。
/// 保存先は Assets/jsons/Ranking/ClearTime.json。
/// </summary>
class RankingBoard
{
public:
    /// ===================================================
    /// public constant
    /// ===================================================

    static constexpr size_t kEntryCount = 3;   // 保持する上位件数
    static constexpr float kEmpty = -1.0f;     // 記録なしを表す値

    /// ===================================================
    /// public method
    /// ===================================================

    /// <summary>
    /// JSON から上位 3 件を読み込む
    /// </summary>
    void Load();

    /// <summary>
    /// タイムを登録する。上位 3 件に入れば JSON へ保存する。
    /// </summary>
    /// <param name="seconds">クリアタイム（秒）</param>
    /// <returns>登録された順位のインデックス(0..2)。ランク外なら -1</returns>
    int AddTime(float seconds);

    /// <summary>
    /// 上位 3 件を取得（速い順。空きスロットは kEmpty）
    /// </summary>
    /// <returns>上位 3 件のタイム配列</returns>
    const std::array<float, kEntryCount>& GetEntries() const { return entries_; }

private:
    /// ===================================================
    /// private method
    /// ===================================================

    /// <summary>
    /// 現在の上位 3 件を JSON へ書き出す
    /// </summary>
    void Save() const;

    /// ===================================================
    /// private variables
    /// ===================================================

    std::array<float, kEntryCount> entries_ = { kEmpty, kEmpty, kEmpty };
};
