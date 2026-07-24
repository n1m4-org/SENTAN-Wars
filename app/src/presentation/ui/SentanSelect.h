#pragma once
#include <array>
#include <memory>
#include <Sprite.h>
#include <presentation/ui/components/SentanSelectCard.h>
#include <presentation/ui/components/SentanSelectInput.h>
#include <utility/layout/FlexContainer.h>

class SentanSelect
{
public:
    SentanSelect();
    ~SentanSelect();
    void Update();

private:
    void InitializeSprite();
    void InitializeCards();
    void InitializeComponents();
    void InitializeContainer();
    void ApplyLayout();
    void RandomPickSentanIds();
    
    std::array<SentanId, 3> pickedIds_;

    std::unique_ptr<SentanSelectInput> pInput_;
    std::unique_ptr<Hagine::Sprite> pSpriteWindow_ = nullptr;
    std::array<std::unique_ptr<SentanSelectCard>, 3> cards_;

    FlexContainer flexContainer_;
    FlexBox flexBox_;
    std::array<FlexItem, 3> flexItems_;
};