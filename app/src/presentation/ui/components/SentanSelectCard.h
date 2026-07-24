#pragma once
#include <memory>
#include <Sprite.h>
#include <Character/Player/Sentan/SentanDefinition.h>
#include <common/ResourcePath.h>


class SentanSelectCard
{
public:
    SentanSelectCard(SentanId sentanId);
    ~SentanSelectCard();

    void Update(bool isSelected);

    void SetPosition(const Hagine::Vector2& position);

    Hagine::Vector2 GetSize() const { return pCardBg_->GetSize(); }

    /// 選び終えたカードにする（もう選べないことを見て分かるようにする）
    void SetTaken(bool isTaken) { isTaken_ = isTaken; }

    /// 描画リストから外す（実体は残す）
    /// 何度呼んでも問題ない
    void Unregister();

private:
    // アイコンをカードの内側に収めるときの余白（ピクセル）
    static constexpr float kIconPadding_ = 16.0f;

    inline static const char* GetPath(SentanId id)
    {
        if (id == SentanId::Sentan1) return Path::Image::SentanBoots;
        if (id == SentanId::Sentan2) return Path::Image::SentanDouble;
        if (id == SentanId::Sentan3) return Path::Image::SentanDril;
        if (id == SentanId::Sentan4) return Path::Image::SentanRotate;
        if (id == SentanId::Sentan5) return Path::Image::SentanTriple;
        return Path::Image::SentanBoots; // デフォルト
    }

    void InitializeSprites();

    SentanId sentanId_;
    bool isTaken_ = false;
    std::unique_ptr<Hagine::Sprite> pCardBg_;
    std::unique_ptr<Hagine::Sprite> pIcon_;
    std::unique_ptr<Hagine::Sprite> pName_;
};