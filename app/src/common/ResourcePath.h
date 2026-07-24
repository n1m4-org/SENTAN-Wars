#pragma once

namespace Path::Image
{
    inline constexpr const char* kDebugMeasure = "measure_2px.png";
    inline constexpr const char* kLogo = "common/logo_580x160.png";
    inline constexpr const char* kStartKey = "common/space_485x120.png";
    inline constexpr const char* StartPrompt = "prompt/start_163x53.png";
    inline constexpr const char* NextWavePrompt = "prompt/next_wave_prompt.png";
    inline constexpr const char* Hp = "hud/HP_53x25.png";
    inline constexpr const char* Bar = "hud/Bar_296x16.png";
    inline constexpr const char* BarRed = "hud/BarRed_296x16.png";
    inline constexpr const char* Remaining = "hud/remaining.png";
    inline constexpr const char* EnemyUnit = "hud/enemy_unit.png";
    inline constexpr const char* Card = "sentan_select/card.png";
    inline constexpr const char* CardBg = "sentan_select/card_bg.png";
    inline constexpr const char* SelectWindow = "sentan_select/window.png";
    inline constexpr const char* Numbers[10] = {
        "common/0_25x25.png",
        "common/1_10x25.png",
        "common/2_25x25.png",
        "common/3_25x25.png",
        "common/4_25x25.png",
        "common/5_25x25.png",
        "common/6_25x25.png",
        "common/7_25x25.png",
        "common/8_25x25.png",
        "common/9_25x25.png"
    };

    inline constexpr const char* RankingNumbers[10] = {
        "common/0_25x25.png",
        "common/1_25x25.png",
        "common/2_25x25.png",
        "common/3_25x25.png",
        "common/4_25x25.png",
        "common/5_25x25.png",
        "common/6_25x25.png",
        "common/7_25x25.png",
        "common/8_25x25.png",
        "common/9_25x25.png"
    };

    inline constexpr const char* SentanBoots = "common/boots.png";
    inline constexpr const char* SentanDouble = "common/double.png";
    inline constexpr const char* SentanDril = "common/dril.png";
    inline constexpr const char* SentanRotate = "common/rotate.png";
    inline constexpr const char* SentanTriple = "common/triple.png";

    inline constexpr const char* Wave = "common/wave_100x25.png";
    inline constexpr const char* Colon = "common/colon_25x25.png";
    inline constexpr const char* Dot = "common/dot_25x25.png";
    inline constexpr const char* ClearTime = "common/ClearTime_500x90.png";
    inline constexpr const char* kWhite1x1 = "debug/white1x1.png";
}

namespace Path::Sound
{
    /// BGM (ループ再生)
    inline constexpr const char* Bgm = "BGM_2.mp3";

    /// 決定音
    inline constexpr const char* Decision = "decision.mp3";

    /// ポータルに入ったときの音
    inline constexpr const char* Portal = "portal.mp3";

    /// 武器を振ったときの音 (この中から1つ選んで鳴らす)
    inline constexpr const char* Swings[] = {
        "swing_1.mp3",
        "swing_2.mp3",
        "swing_3.wav"
    };

    /// 攻撃がヒットしたときの音 (この中から1つ選んで鳴らす)
    inline constexpr const char* AttackHits[] = {
        "attack_1.mp3",
        "attack_2.mp3"
    };
}