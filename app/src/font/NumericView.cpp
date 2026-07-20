#include "NumericView.h"
#include <cmath>
#include <cassert>
#include <Graphics/Texture/TextureManager.h>
#include <SpriteManager.h>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void NumericView::Initialize(std::span<TextureHandleType> textureHandles, const std::string& name)
{
    assert(textureHandles.size() == kRadix_);

    // デバッグ用の名前を設定
    debugName_ = "NumericView_" + name;

    // デバッグUIに登録
    #ifdef _DEBUG
    debugEntry.RegisterCustomGuiFunction("", std::bind(&NumericView::ImGui, this));
    #endif // _DEBUG

    // テクスチャハンドルを保存
    for (uint32_t i = 0; i < kRadix_; ++i)
    {
        numberTextureHandles_[i] = textureHandles[i];
    }
    // 最低1桁分のスプライトは用意しておく
    this->AddSprite();
}

void NumericView::Finalize()
{
}

void NumericView::Update()
{
    /// スプライトの数を確認
    uint32_t number = currentNumber_;
    /// 桁数を計算
    const uint32_t digitCount = this->GetDigitCount();

    /// 桁が多くなり、スプライトが足りない場合は追加する
    if (numberSprites_.size() < digitCount)
    {
        uint32_t countToAdd = digitCount - static_cast<uint32_t>(numberSprites_.size());
        for (uint32_t i = 0; i < countToAdd; ++i)
        {
            this->AddSprite();
        }
    }

    /// 各スプライトに数字を割り当てる
    /// スプライト・サイズともに「左端(最上位桁)=index 0」の並びで格納し、
    /// FontLayout::Compute の並び順(左→右)と一致させる
    glyphInfos_.assign(digitCount, GlyphInfo{});
    for (uint32_t i = 0; i < digitCount; ++i)
    {
        uint32_t digit = number % 10;
        const uint32_t spriteIndex = digitCount - i - 1;
        TextureHandleType handle = numberTextureHandles_[digit];
        numberSprites_[spriteIndex]->SetTexturePath(handle);
        /// サイズを設定
        const auto& metadata = Hagine::TextureManager::GetInstance()->GetMetaData(handle);
        const float aspect = static_cast<float>(metadata.width) / static_cast<float>(metadata.height);
        Hagine::Vector2 size = { fontSizeY_ * aspect, static_cast<float>(fontSizeY_) };
        glyphInfos_[spriteIndex].size = size;
        number /= 10;
    }

    /// レイアウト計算
    auto layoutResults = fontLayout_.Compute(glyphInfos_);
    for (uint32_t i = 0; i < digitCount; ++i)
    {
        numberSprites_[i]->SetColor({ color_.x, color_.y,color_.z });
        numberSprites_[i]->SetPosition(layoutResults[i].leftTop);
        numberSprites_[i]->SetSize(glyphInfos_[i].size);
    }

    auto sm = Hagine::SpriteManager::GetInstance();
    for (uint32_t i = 0; i < numberSprites_.size(); ++i)
    {
        if (i >= digitCount)
        {
            auto& sprite = numberSprites_[i];
            sm->UnregisterExternal(sprite.get());
        }
        else
        {
            auto& sprite = numberSprites_[i];
            sm->RegisterExternal(sprite.get());
        }
    }
}

void NumericView::ImGui()
{
    #ifdef _DEBUG

    ImGui::DragFloat2("FontLayout LeftTop", &fontLayout_.GetProperties().leftTop.x, 1.0f);
    ImGui::DragFloat("FontLayout LetterSpacing", &fontLayout_.GetProperties().letterSpacing, 0.1f);
    ImGui::DragFloat2("FontLayout AnchorPoint", &fontLayout_.GetProperties().anchorPoint.x, 0.01f);
    ImGui::DragFloat("FontSize", &fontSizeY_, 0.01f, 1.0f, 1000.0f);
    ImGui::DragScalar("Current Number", ImGuiDataType_U32, &currentNumber_);

    #endif // _DEBUG
}

void NumericView::SetFontSize(float sizeY)
{
    fontSizeY_ = sizeY;
}

void NumericView::SetColor(const Hagine::Vector4& color)
{
    color_ = color;
}

void NumericView::AddSprite()
{
    auto sprite = std::make_unique<Hagine::Sprite>();
    sprite->Initialize("debug/white1x1.png", {});
    numberSprites_.emplace_back(std::move(sprite));
}

uint32_t NumericView::GetDigitCount() const
{
    if (currentNumber_ == 0) return 1;
    else return static_cast<uint32_t>(std::log10(currentNumber_)) + 1;
}
