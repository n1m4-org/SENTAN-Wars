#pragma once
#include <array>
#include <functional>
#include <memory>
#include <Sprite.h>
#include <presentation/ui/components/SentanSelectCard.h>
#include <presentation/ui/components/SentanSelectInput.h>
#include <utility/layout/FlexContainer.h>

class SentanSelect
{
public:
    /// 決定されたSENTANを受け取るコールバック
    using DecideCallback = std::function<void(SentanId)>;

    /// pickCount は選ばせる枚数（プレイヤーがあと何個SENTANを付けられるか）
    explicit SentanSelect(uint32_t pickCount);
    ~SentanSelect();
    void Update();

    /// 1枚決まるたびに呼ばれる処理を設定する
    /// 選んだSENTANをどうするかはこのクラスの外が決める
    void SetOnDecideCallback(DecideCallback callback) { onDecideCallback_ = std::move(callback); }

    /// 選ぶ枚数を選び終えたか（閉じるタイミングを外が決められるようにする）
    bool IsDecided() const { return decidedCount_ >= pickCount_; }

private:
    /// index の次に選べるカードを探す（無ければ index をそのまま返す）
    uint32_t FindSelectableIndex(uint32_t index) const;

    void InitializeSprite();
    void InitializeCards();
    void InitializeComponents();
    void InitializeContainer();
    void ApplyLayout();
    void RandomPickSentanIds();
    
    std::array<SentanId, 3> pickedIds_{};

    // 選ばせる枚数と、選び終えた枚数
    uint32_t pickCount_ = 1;
    uint32_t decidedCount_ = 0;

    // すでに選んだカードかどうか（同じカードを選び直せないようにする）
    std::array<bool, 3> isTaken_{};

    // 決定を知らせる先
    DecideCallback onDecideCallback_ = nullptr;

    std::unique_ptr<SentanSelectInput> pInput_;
    std::unique_ptr<Hagine::Sprite> pSpriteWindow_ = nullptr;
    std::array<std::unique_ptr<SentanSelectCard>, 3> cards_;

    FlexContainer flexContainer_;
    FlexBox flexBox_;
    std::array<FlexItem, 3> flexItems_;
};