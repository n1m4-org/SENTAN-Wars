#include "TitleScene.h"
#include <scene/SceneRegistry.h>
#include <type/Vector4.h>
#include <utility/ViewportUnits.hpp>
#include <SpriteManager.h>
#include <utility/SpriteUnregisterer.h>
#include <audio/SoundPlayer.h>


REGISTER_SCENE("TITLE", TitleScene)

using namespace Hagine;

void TitleScene::Initialize()
{
	// シーン共通の初期化処理
	BaseScene::Initialize();
	vp_.Initialize("ClearCamera");
	pLightGroup_->LoadLightData("GameLight");
	pObjectManager_->LoadAll("GameScene");
	pDrawSystem_->Register("Title_PreDraw", DrawLayer::PreEffect, [this](const ViewProjection& vp)
		{
			pObjectManager_->Draw(vp);
		});
	pDrawSystem_->Register("Title_PostDraw", DrawLayer::PostEffect, [this](const ViewProjection& vp)
		{
			pSpriteManager_->DrawAll();
		});


	// スプライトの生成と初期化
	this->InitializeSprites();

	// フレックスコンテナの初期化
	this->InitializeFlexContainer();
}

void TitleScene::Finalize()
{
	utl::sprite::Unregister(sprites_);
}

void TitleScene::Update()
{
	auto result = pFlexContainer_->Calculate(containerBox_, flexItems_);

	for (size_t i = 0; i < result.size(); ++i)
	{
		auto& sprite = sprites_[i + 1]; // Logoは除外するため+1
		sprite->SetPosition(result[i].position);
		sprite->SetSize(result[i].size);
	}

	pContainerArea_->SetPosition(containerBox_.position);
	pContainerArea_->SetSize(containerBox_.size);

	this->UpdateStartKeyColor();

	vp_.UpdateMatrix();
}

void TitleScene::InitializeSprites()
{
	static constexpr const char* kLogo = "common/logo_580x160.png";
	static constexpr const char* kStartKey = "common/space_485x120.png";
	static constexpr const char* kPrompt = "prompt/start_163x53.png";
	/// TODO: Vector4をconstexprにする
	static const Vector4 kWhite = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	pContainerArea_ = std::make_unique<Sprite>();
	pContainerArea_->Initialize("debug/white1x1.png", {}, { 0.0f, 0.0f, 0.0f, 0.5f }, {});
	
	/// ロゴ (タイトル)
	{
		auto& sprite = GetSprite(SpriteName::Logo);
		sprite = std::make_unique<Sprite>();
		Vector2 position = { 50_vw, 25_vh };
		sprite->Initialize(kLogo, position, kWhite, { 0.5f, 0.5f });
	}

	/// スペースキー
	{
		auto& sprite = GetSprite(SpriteName::StartKey);
		sprite = std::make_unique<Sprite>();
		sprite->Initialize(kStartKey, {}, kWhite, {});
		const auto& size = sprite->GetSize();
		sprite->SetSize(size * kStartKeyScale_);
		flexItems_.push_back({ sprite->GetSize() });
	}

	/// プロンプト
	{
		auto& sprite = GetSprite(SpriteName::Prompt);
		sprite = std::make_unique<Sprite>();
		sprite->Initialize(kPrompt, {}, kWhite, {});
		const auto& size = sprite->GetSize();
		sprite->SetSize(size * kPromptScale_);
		flexItems_.push_back({ sprite->GetSize() });
	}

	/// スプライトをすべてSpriteManagerに登録
	SpriteManager* sm = SpriteManager::GetInstance();
	sm->RegisterExternal(pContainerArea_.get());
	for (auto& sprite : sprites_)
	{
		sm->RegisterExternal(sprite.get());
	}
}

void TitleScene::InitializeFlexContainer()
{
	pFlexContainer_ = std::make_unique<FlexContainer>();
	pFlexContainer_->direction_ = FlexDirection::Row;
	pFlexContainer_->justifyContent_ = JustifyContent::Center;
	pFlexContainer_->alignItems_ = AlignItems::Center;
	pFlexContainer_->gap_ = 16.0f;

	containerBox_.position = Hagine::Vector2(0.0f, 650.0f);
	containerBox_.size = Hagine::Vector2(1760.0f, 80.0f);
}

void TitleScene::UpdateStartKeyColor()
{
	// 押した瞬間だけ決定音を鳴らす（押しっぱなしで鳴り続けないようトリガーで見る）
	if (pInput_->TriggerKey(DIK_SPACE))
	{
		SoundPlayer::GetInstance()->PlayDecision();
	}

	if (pInput_->PushKey(DIK_SPACE))
	{
		this->GetSprite(SpriteName::StartKey)->SetColor({ 0.7f, 0.7f, 0.7f });
	} else
	{
		this->GetSprite(SpriteName::StartKey)->SetColor({ 1.0f, 1.0f, 1.0f });
	}
}
