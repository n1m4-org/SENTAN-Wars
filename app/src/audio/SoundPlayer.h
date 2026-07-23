#pragma once
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>

/// ゲーム中の音を鳴らす窓口
///
/// 「どのファイルを」「どのくらいの音量で」鳴らすかはここに閉じ込め、
/// 呼ぶ側は PlaySwing() のように「何が起きたか」だけを伝える。
/// 候補が複数ある音 (振り・ヒット) はこちらでランダムに選ぶので、
/// 呼ぶ側は音が何種類あるかを知らなくてよい
class SoundPlayer
{
public:
    /// ===================================================
    /// public method
    /// ===================================================

    static SoundPlayer* GetInstance();

    /// <summary>
    /// 使う音をまとめて読み込む
    /// 鳴らす瞬間に読み込むとその場で処理落ちするため、先に済ませておく
    /// </summary>
    void Initialize();

    /// <summary>
    /// BGMを流し始める (既に流れていれば何もしない)
    /// </summary>
    void PlayBgm();

    /// <summary>
    /// BGMを止める
    /// </summary>
    void StopBgm();

    /// <summary>
    /// 決定音を鳴らす (タイトルでスペースを押したときなど)
    /// </summary>
    void PlayDecision();

    /// <summary>
    /// 武器を振った音を鳴らす
    /// </summary>
    void PlaySwing();

    /// <summary>
    /// 攻撃がヒットした音を鳴らす
    /// </summary>
    void PlayAttackHit();

    /// <summary>
    /// ポータルに入った音を鳴らす
    /// </summary>
    void PlayPortal();

private:
    /// ===================================================
    /// private method
    /// ===================================================

    SoundPlayer() = default;
    ~SoundPlayer() = default;
    SoundPlayer(const SoundPlayer&) = delete;
    SoundPlayer& operator=(const SoundPlayer&) = delete;

    /// <summary>
    /// 音声番号を引く (未読込ならここで読み込む)
    /// </summary>
    /// <param name="fileName">サウンドフォルダからの相対パス</param>
    /// <returns>uint32_t: 音声番号 (読み込めなければ kInvalidHandle_)</returns>
    uint32_t GetHandle(const char* fileName);

    /// <summary>
    /// 効果音を1回鳴らす
    /// </summary>
    /// <param name="fileName">鳴らすファイル</param>
    /// <param name="volume">音量 (0.0〜1.0)</param>
    void PlaySe(const char* fileName, float volume);

    /// <summary>
    /// 候補の中から1つ選んで鳴らす
    /// 同じ音が続くと単調に聞こえるため、直前と同じものは選ばない
    /// </summary>
    /// <param name="fileNames">候補</param>
    /// <param name="volume">音量 (0.0〜1.0)</param>
    void PlayAnyOf(std::span<const char* const> fileNames, float volume);

    /// ===================================================
    /// private variables
    /// ===================================================

    // 読み込みに失敗した / まだ流していないことを表す音声番号
    static constexpr uint32_t kInvalidHandle_ = UINT32_MAX;

    // 音量。BGMは効果音を埋もれさせないよう控えめにしてある
    static constexpr float kBgmVolume_ = 0.12f;
    static constexpr float kDecisionVolume_ = 0.35f;
    static constexpr float kSwingVolume_ = 0.25f;
    static constexpr float kAttackHitVolume_ = 0.35f;
    static constexpr float kPortalVolume_ = 0.35f;

    // ファイル名 → 音声番号 (同じファイルを二重に読み込まないため)
    std::unordered_map<std::string, uint32_t> handles_;

    // 流しているBGMの音声番号 (流していなければ kInvalidHandle_)
    uint32_t bgmHandle_ = kInvalidHandle_;

    // 候補リストごとに、直前に選んだ番号を覚えておく (キーは候補リストの先頭アドレス)
    std::unordered_map<const void*, size_t> lastPickedIndices_;
};
