#include "ClearTimeView.h"
#include <algorithm>
#include <cmath>
#include <common/ResourcePath.h>
#include <SpriteManager.h>
#include <Graphics/Texture/TextureManager.h>
#include <utility/SpriteUnregisterer.h>

using namespace Hagine;

void ClearTimeView::Initialize()
{
    // 使用するグリフのテクスチャを事前に読み込む（メタデータ取得のため）
    TextureManager* tm = TextureManager::GetInstance();
    for (uint32_t glyph = 0u; glyph <= kColonGlyph_; ++glyph)
    {
        tm->LoadTexture(this->GetGlyphPath(glyph));
    }

    // スロットごとにスプライトを生成し、初期グリフを設定する
    SpriteManager* sm = SpriteManager::GetInstance();
    for (size_t i = 0; i < kSlotCount_; ++i)
    {
        sprites_[i] = std::make_unique<Sprite>();
        sprites_[i]->Initialize(this->GetGlyphPath(glyphs_[i]), {}, color_, {});
        appliedGlyphs_[i] = glyphs_[i];
        sm->RegisterExternal(sprites_[i].get());
    }
}

void ClearTimeView::Finalize()
{
    utl::sprite::Unregister(sprites_);
}

void ClearTimeView::Update()
{
    float x = origin_.x;
    const float y = origin_.y;

    for (size_t i = 0; i < kSlotCount_; ++i)
    {
        auto& sprite = sprites_[i];
        const uint32_t glyph = glyphs_[i];

        // グリフが変わった場合のみテクスチャを差し替える
        if (glyph != appliedGlyphs_[i])
        {
            sprite->SetTexturePath(this->GetGlyphPath(glyph));
            appliedGlyphs_[i] = glyph;
        }

        // アスペクト比を維持したままフォントサイズ（高さ）に合わせる
        const float aspect = this->GetGlyphAspect(glyph);
        const Vector2 size = { fontSize_ * aspect, fontSize_ };

        sprite->SetPosition({ x, y });
        sprite->SetSize(size);
        sprite->SetColor({ color_.x, color_.y, color_.z });
        sprite->SetAlpha(color_.w);

        x += size.x;
        if (i + 1 < kSlotCount_)
        {
            x += letterSpacing_;
        }
    }

    overallSize_ = { x - origin_.x, fontSize_ };
}

void ClearTimeView::SetSeconds(float seconds)
{
    // 負値をクランプし、MM:SS に分解する
    const int totalSeconds = static_cast<int>(std::floor((std::max)(0.0f, seconds)));
    const int minutes = (totalSeconds / 60) % 100; // 分は 2 桁まで
    const int secondsPart = totalSeconds % 60;

    glyphs_[static_cast<size_t>(Slot::MinTens)] = static_cast<uint32_t>(minutes / 10);
    glyphs_[static_cast<size_t>(Slot::MinOnes)] = static_cast<uint32_t>(minutes % 10);
    glyphs_[static_cast<size_t>(Slot::Colon)] = kColonGlyph_;
    glyphs_[static_cast<size_t>(Slot::SecTens)] = static_cast<uint32_t>(secondsPart / 10);
    glyphs_[static_cast<size_t>(Slot::SecOnes)] = static_cast<uint32_t>(secondsPart % 10);
}

const char* ClearTimeView::GetGlyphPath(uint32_t glyph) const
{
    if (glyph == kColonGlyph_)
    {
        return Path::Image::Colon;
    }
    return Path::Image::Numbers[glyph];
}

float ClearTimeView::GetGlyphAspect(uint32_t glyph) const
{
    const auto& metadata = TextureManager::GetInstance()->GetMetaData(this->GetGlyphPath(glyph));
    return static_cast<float>(metadata.width) / static_cast<float>(metadata.height);
}
