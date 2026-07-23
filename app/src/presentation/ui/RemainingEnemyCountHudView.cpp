#include "RemainingEnemyCountHudView.h"
#include <common/ResourcePath.h>
#include <utility/ViewportUnits.hpp>


RemainingEnemyCountHudView::RemainingEnemyCountHudView()
{
    this->InitializeFlexContainers();
    this->InitializeNumericView();
    this->InitializeSprites();

    // 位置を確定
    this->ApplyFlexLayout();
}

void RemainingEnemyCountHudView::Update()
{
    this->ApplyFlexLayout();

    pNvCount_->Update();
}

void RemainingEnemyCountHudView::InitializeFlexContainers()
{
    flexBoxOverall_.position =
    {
        85_vw,
        50_vh
    };

    containerOverall_.SetName("RemainingEnemy::Overall");
    containerOverall_.direction_ = FlexDirection::Row;
    containerOverall_.justifyContent_ = JustifyContent::Center;
    containerOverall_.alignItems_ = AlignItems::Center;
    containerOverall_.gap_ = 12.0f;

    containerCount_.SetName("RemainingEnemy::Count");
    containerCount_.direction_ = FlexDirection::Row;
    containerCount_.justifyContent_ = JustifyContent::Center;
    containerCount_.alignItems_ = AlignItems::Center;
    containerCount_.gap_ = 12.0f;

    flexItemsOverall_.resize(2);
    flexItemsCount_.resize(2);
}

void RemainingEnemyCountHudView::InitializeNumericView()
{
    std::vector<std::string> textureHandles;
    for (const auto& path : Path::Image::Numbers)
    {
        textureHandles.emplace_back(path);
    }

    pNvCount_ = std::make_unique<NumericView>();
    pNvCount_->Initialize(textureHandles, "EnemyCounter");
    pNvCount_->GetFontLayoutProperties().letterSpacing = 4.0f;
}

void RemainingEnemyCountHudView::InitializeSprites()
{
    pSpriteRemain_ = std::make_unique<Hagine::Sprite>();
    pSpriteRemain_->Initialize(Path::Image::Remaining, {});

    pSpriteUnit_ = std::make_unique<Hagine::Sprite>();
    pSpriteUnit_->Initialize(Path::Image::EnemyUnit, {});

    Hagine::SpriteManager::GetInstance()->RegisterExternal(pSpriteRemain_.get());
    Hagine::SpriteManager::GetInstance()->RegisterExternal(pSpriteUnit_.get());
}

void RemainingEnemyCountHudView::ApplyFlexLayout()
{
    ///「n 人」のレイアウトを調整
    flexItemsCount_[0] = { pNvCount_->GetSizeOverall() };
    flexItemsCount_[1] = { pSpriteUnit_->GetSize() };

    // コンテナのサイズを計算
    auto sizeCount_ = containerCount_.ContainerSize(flexItemsCount_);

    // コンテナのサイズを設定
    flexBoxCount_.size = sizeCount_;

    // コンテナのレイアウトを計算
    auto resultCount_ = containerCount_.Calculate(flexBoxCount_, flexItemsCount_);

    // コンテナ位置の適用
    pNvCount_->GetFontLayoutProperties().leftTop = resultCount_[0].position;
    pSpriteUnit_->SetPosition(resultCount_[1].position);

    /// 「残り」 「n 人」のレイアウトを調整
    flexItemsOverall_[0] = { pSpriteRemain_->GetSize() };
    flexItemsOverall_[1] = { flexBoxCount_.size };

    // コンテナのサイズを計算
    auto sizeOverall_ = containerOverall_.ContainerSize(flexItemsOverall_);

    // コンテナのサイズを設定
    flexBoxOverall_.size = sizeOverall_;

    // コンテナのレイアウトを計算
    auto resultOverall = containerOverall_.Calculate(flexBoxOverall_, flexItemsOverall_);

    // コンテナ位置の適用
    pSpriteRemain_->SetPosition(resultOverall[0].position);
    flexBoxCount_.position = resultOverall[1].position;
}
