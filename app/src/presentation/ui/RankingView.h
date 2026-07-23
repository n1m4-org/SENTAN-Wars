#pragma once
#include <array>
#include <cstddef>
#include <memory>
#include <Sprite.h>
#include <type/Vector2.h>
#include <type/Vector4.h>
#include <presentation/ui/ClearTimeView.h>
#include <logic/ranking/RankingBoard.h>

/// <summary>
/// クリアタイムのランキング（上位 RankingBoard::kEntryCount 件）を描画するビュー。
/// 各行は「順位番号 + ドット + タイム」（例: 1. 00:00:00）で表示する。
/// 上位 3 位を金・銀・銅（抑えめ）、それ以降も同系色で色分けし、
/// 新記録が入るときは前の記録を下へ突き落とすスライド演出、
/// その後に挿入行を拡縮でアピールするパルス演出を行う。
/// 表示件数は RankingBoard::kEntryCount に追従する。
/// </summary>
class RankingView
{
public:
    // 表示する順位数（＝記録件数）。RankingBoard::kEntryCount に追従する
    static constexpr size_t kRankCount = RankingBoard::kEntryCount;

    /// <summary>
    /// レイアウト設定
    /// </summary>
    struct Config
    {
        float leftX = 0.0f;                              // 左揃えの基準 X
        std::array<float, kRankCount> slotCenterY = {};  // 各順位行の縦中心
        float fontSize = 44.0f;                          // タイムの高さ
        float numberGap = 18.0f;                         // 順位番号とタイムの間隔
        float rowSpacing = 60.0f;                        // 行間（落下距離に使用）
    };

    /// <summary>初期化（スプライトの生成・登録）</summary>
    /// <param name="config">レイアウト設定</param>
    void Initialize(const Config& config);

    /// <summary>終了処理（スプライトの登録解除）</summary>
    void Finalize();

    /// <summary>更新（演出の進行とレイアウト再計算）</summary>
    /// <param name="deltaTime">経過時間</param>
    void Update(float deltaTime);

    /// <summary>アニメ無しで指定の順位表を表示する</summary>
    /// <param name="entries">上位 kRankCount 件（速い順。空きは負値）</param>
    void ShowStatic(const std::array<float, kRankCount>& entries);

    /// <summary>
    /// 挿入演出を開始する（old → new へ、insertRank の位置に突き落とし）
    /// </summary>
    /// <param name="oldEntries">挿入前の上位 kRankCount 件</param>
    /// <param name="newEntries">挿入後の上位 kRankCount 件</param>
    /// <param name="insertRank">今回のタイムが入った順位(0..kRankCount-1)</param>
    void StartInsert(const std::array<float, kRankCount>& oldEntries, const std::array<float, kRankCount>& newEntries, int insertRank);

    /// <summary>演出を即座に最終状態へ飛ばす（スキップ用）</summary>
    /// <param name="newEntries">最終的な上位 kRankCount 件</param>
    void SkipToEnd(const std::array<float, kRankCount>& newEntries);

    /// <summary>出現スケール(0..1)を設定する</summary>
    /// <param name="scale">出現スケール</param>
    void SetRevealScale(float scale) { revealScale_ = scale; }

    /// <summary>スライド・パルス演出が完了しているか</summary>
    /// <returns>bool: 演出中でなければ true</returns>
    bool IsFinished() const { return mode_ == Mode::Static || mode_ == Mode::Done; }

private:
    /// <summary>演出モード</summary>
    enum class Mode { Static, Sliding, Pulsing, Done };

    static constexpr size_t kSlotCount_ = kRankCount;      // 順位数
    static constexpr size_t kCardCount_ = kSlotCount_ + 1; // 同時に存在しうるタイム札（全スロット + 落下分）

    static constexpr float kSlideDuration_ = 0.45f;   // 突き落としスライドの時間
    static constexpr float kPulseDuration_ = 0.5f;    // パルスの時間
    static constexpr float kPulseAmplitude_ = 0.35f;  // パルスの拡大量
    static constexpr float kNumberDotGap_ = 4.0f;     // 順位番号とドットの間隔

    /// <summary>スライドで動くタイム札</summary>
    struct Card
    {
        float value = -1.0f;              // 表示する秒数
        bool active = false;              // 描画対象か
        bool inserted = false;            // 今回挿入された札か（パルス対象）
        float startY = 0.0f;              // スライド開始 Y
        float endY = 0.0f;                // スライド終了 Y
        float currentY = 0.0f;            // 現在 Y
        Hagine::Vector4 startColor = {};  // スライド開始色
        Hagine::Vector4 endColor = {};    // スライド終了色
        Hagine::Vector4 currentColor = {};// 現在色
    };

    /// <summary>各スプライトの位置・サイズ・色を現在の状態から反映する</summary>
    void LayoutFrame();

    /// <summary>順位の色（金・銀・銅）を取得する</summary>
    const Hagine::Vector4& MedalColor(size_t rank) const;

    Config config_ = {};

    Mode mode_ = Mode::Static;
    int insertRank_ = -1;         // 今回挿入された順位（パルス対象）。-1 は無し
    float animTimer_ = 0.0f;      // スライド用タイマー
    float pulseTimer_ = 0.0f;     // パルス用タイマー
    float revealScale_ = 0.0f;    // 出現スケール
    float numberAdvance_ = 0.0f;  // 順位番号 + 間隔（タイム左端のオフセット）

    std::array<ClearTimeView, kCardCount_> cardViews_ = {};                              // タイムの描画
    std::array<Card, kCardCount_> cardData_ = {};                                        // タイム札の状態
    std::array<std::unique_ptr<Hagine::Sprite>, kSlotCount_> numberSprites_ = {};        // 順位番号（1,2,3）
    std::array<std::unique_ptr<Hagine::Sprite>, kSlotCount_> dotSprites_ = {};           // 順位番号の後のドット
    std::array<Hagine::Vector2, kSlotCount_> numberFullSize_ = {};                       // 順位番号の最終サイズ
    Hagine::Vector2 dotFullSize_ = {};                                                   // ドットの最終サイズ
    std::array<bool, kSlotCount_> slotHasNumber_ = {};                                   // その順位に番号を表示するか
};
