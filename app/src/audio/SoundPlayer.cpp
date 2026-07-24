#include "SoundPlayer.h"
#include <Audio.h>
#include <common/ResourcePath.h>
#include <random.h>

using namespace Hagine;

SoundPlayer* SoundPlayer::GetInstance()
{
    static SoundPlayer instance;
    return &instance;
}

void SoundPlayer::Initialize()
{
    // 使う音を先に読み込んでおく (読み込み結果は handles_ に載る)
    GetHandle(Path::Sound::Bgm);
    GetHandle(Path::Sound::Decision);
    GetHandle(Path::Sound::Portal);
    for (const char* fileName : Path::Sound::Swings)
    {
        GetHandle(fileName);
    }
    for (const char* fileName : Path::Sound::AttackHits)
    {
        GetHandle(fileName);
    }
}

void SoundPlayer::PlayBgm()
{
    // 二重に流すと音が重なってしまう
    if (bgmHandle_ != kInvalidHandle_)
    {
        return;
    }

    const uint32_t handle = GetHandle(Path::Sound::Bgm);
    if (handle == kInvalidHandle_)
    {
        return;
    }

    Audio::GetInstance()->PlayWave(handle, kBgmVolume_, true);
    bgmHandle_ = handle;
}

void SoundPlayer::StopBgm()
{
    if (bgmHandle_ == kInvalidHandle_)
    {
        return;
    }

    Audio::GetInstance()->StopWave(bgmHandle_);
    bgmHandle_ = kInvalidHandle_;
}

void SoundPlayer::PlayDecision()
{
    PlaySe(Path::Sound::Decision, kDecisionVolume_);
}

void SoundPlayer::PlaySwing()
{
    PlayAnyOf(Path::Sound::Swings, kSwingVolume_);
}

void SoundPlayer::PlayAttackHit()
{
    PlayAnyOf(Path::Sound::AttackHits, kAttackHitVolume_);
}

void SoundPlayer::PlayPortal()
{
    PlaySe(Path::Sound::Portal, kPortalVolume_);
}

uint32_t SoundPlayer::GetHandle(const char* fileName)
{
    if (!fileName)
    {
        return kInvalidHandle_;
    }

    // 一度読んだファイルは覚えておき、読み込み直さない
    const std::string key = fileName;
    auto it = handles_.find(key);
    if (it != handles_.end())
    {
        return it->second;
    }

    // 読み込みに失敗した場合も覚えておく (毎回読み直して詰まらせないため)
    const uint32_t handle = Audio::GetInstance()->LoadWave(key);
    handles_.emplace(key, handle);
    return handle;
}

void SoundPlayer::PlaySe(const char* fileName, float volume)
{
    const uint32_t handle = GetHandle(fileName);
    if (handle == kInvalidHandle_)
    {
        return;
    }

    Audio::GetInstance()->PlayWave(handle, volume, false);
}

void SoundPlayer::PlayAnyOf(std::span<const char* const> fileNames, float volume)
{
    if (fileNames.empty())
    {
        return;
    }

    const size_t count = fileNames.size();
    size_t index = static_cast<size_t>(Random::Range(0, static_cast<int>(count) - 1));

    // 直前と同じものを引いたら1つ隣にずらす (候補が1つしかなければそのまま)
    if (count > 1)
    {
        auto it = lastPickedIndices_.find(fileNames.data());
        if (it != lastPickedIndices_.end() && it->second == index)
        {
            index = (index + 1) % count;
        }
    }
    lastPickedIndices_[fileNames.data()] = index;

    PlaySe(fileNames[index], volume);
}
