#include "ClearScene.h"
#include <algorithm>
#include <common/ResourcePath.h>
#include <Easing.h>
#include <Frame.h>
#include <scene/SceneRegistry.h>
#include <scene/SceneManager.h>
#include <SpriteManager.h>
#include <utility/SpriteUnregisterer.h>
#include <WinApp.h>

REGISTER_SCENE("CLEAR", ClearScene)

using namespace Hagine;

void ClearScene::Initialize()
{

	// シーン共通の初期化処理
	BaseScene::Initialize();
	vp_.Initialize("ClearCamera");
	pLightGroup_->LoadLightData("GameLight");
	pObjectManager_->LoadAll("GameScene");

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);


	// オブジェクトの描画を登録する
	pDrawSystem_->Register("Clear_PreDraw", DrawLayer::PreEffect, [this](const ViewProjection& vp)
		{
			pObjectManager_->Draw(vp);
		});

	// スプライトの描画を登録する
	pDrawSystem_->Register("Clear_PostDraw", DrawLayer::PostEffect, [this](const ViewProjection& vp)
		{
			pSpriteManager_->DrawAll();
		});

	// クリアタイムをゲーム側（SceneManager 経由）から取得する。
	// 0（未計測・CLEAR を直接起動した場合）はデバッグ用のデフォルト値を維持する。
	const float recordedClearTime = pSceneManager_->GetClearTime();
	if (recordedClearTime > 0.0f)
	{
		targetClearSeconds_ = recordedClearTime;
	}

	// ランキングを読み込み、挿入前の上位順位を控える
	ranking_.Load();
	oldRankingEntries_ = ranking_.GetEntries();

	// 実際のプレイのタイムならランキングへ登録する
	// （CLEAR を直接起動しただけのデモ値ではランキングを汚さない）
	if (recordedClearTime > 0.0f)
	{
		myRank_ = ranking_.AddTime(targetClearSeconds_);
	}
	newRankingEntries_ = ranking_.GetEntries();

	// レイアウトとスプライトの初期化
	this->InitializeLayout();
	this->InitializeSprites();

	// 初期状態（枠は展開前、クリアタイムは非表示）を反映する
	phase_ = Phase::FrameExpand;
	this->ApplyReveal(0.0f);
	this->ApplyCountUp(0.0f);
	clearTimeView_.Update();
	rankingView_.Update(0.0f);

	pOffScreen_->LoadData("ClearScenePostEffect");
	
}

void ClearScene::Finalize()
{
	frame_.Finalize();
	clearTimeView_.Finalize();
	rankingView_.Finalize();

	Sprite* labels[] = { pClearTimeLabel_.get() };
	utl::sprite::Unregister(labels);

	pOffScreen_->RemoveEffect(0);
	pOffScreen_->RemoveEffect(1);
}

void ClearScene::Update()
{
	const float deltaTime = Frame::DeltaTime();

	// シーン遷移（フェード）が空けきってから演出を進める。
	// 遷移中に枠の横展開が進んでしまい、ほとんど見えないままになるのを防ぐ。
	if (pSceneManager_->GetTransitionEnd())
	{
		this->UpdateDirection(deltaTime);
	}

	this->UpdateCamera();
	clearTimeView_.Update();
	rankingView_.Update(deltaTime);
}

void ClearScene::UpdateDirection(float deltaTime)
{
	// SPACE で演出をスキップする
	if (phase_ != Phase::Done && pInput_->TriggerKey(DIK_SPACE))
	{
		this->Skip();
	}

	switch (phase_)
	{
	case Phase::FrameExpand:
		// 枠を横 → 縦 の順に展開する
		frame_.Update(deltaTime);
		this->ApplyReveal(0.0f);
		this->ApplyCountUp(0.0f);
		if (frame_.IsFinished())
		{
			phase_ = Phase::Reveal;
		}
		break;

	case Phase::Reveal:
	{
		// クリアタイムを出現（小 → 大）させつつカウントアップする
		revealTimer_ = (std::min)(revealTimer_ + deltaTime, kRevealDuration_);
		countTimer_ = (std::min)(countTimer_ + deltaTime, kCountDuration_);

		const float scale = ApplyEasing<float>(EasingType::OutBack, 0.0f, 1.0f, revealTimer_, kRevealDuration_);
		this->ApplyReveal(scale);

		const float seconds = ApplyEasing<float>(EasingType::OutCubic, 0.0f, targetClearSeconds_, countTimer_, kCountDuration_);
		this->ApplyCountUp(seconds);

		if (revealTimer_ >= kRevealDuration_ && countTimer_ >= kCountDuration_)
		{
			phase_ = Phase::Done;
			// クリアタイムのカウントアップが終わってから、ランキングへ突き落とし挿入する
			this->StartRankingInsert();
		}
		break;
	}

	case Phase::Done:
		// 最終状態を維持する（ランキングのスライド／パルスは rankingView_ が進行させる）
		this->ApplyReveal(1.0f);
		this->ApplyCountUp(targetClearSeconds_);
		break;
	}
}

void ClearScene::AddSceneSetting() {
	debugCamera_->imgui();
	vp_.ShowDebugInfo();
}

void ClearScene::InitializeLayout()
{
	// 枠は画面中央に配置する
	const float screenWidth = static_cast<float>(WinApp::GetVirtualWidth());
	const float screenHeight = static_cast<float>(WinApp::GetVirtualHeight());

	frameSize_ = { 1120.0f, 680.0f };
	frameCenter_ = { screenWidth * 0.5f, screenHeight * 0.5f };

	const Vector2 topLeft = frameCenter_ - frameSize_ * 0.5f;

	// クリアタイムは枠の左上に配置する
	constexpr float kPadding = 56.0f;
	constexpr float kGap = 28.0f;

	leftX_ = topLeft.x + kPadding;
	labelCenterY_ = topLeft.y + kPadding + kLabelHeight_ * 0.5f;
	timeCenterY_ = labelCenterY_ + kLabelHeight_ * 0.5f + kGap + timeFullFontSize_ * 0.5f;

	// ランキングはクリアタイムの下に上位順に並べる
	const float rankTop = timeCenterY_ + timeFullFontSize_ * 0.5f + kRankTopGap_;
	for (size_t i = 0; i < kRankRowCount_; ++i)
	{
		rankRowCenterY_[i] = rankTop + i * kRankRowSpacing_ + kRankFontSize_ * 0.5f;
	}
}

void ClearScene::InitializeSprites()
{
	// 枠（横 → 縦 に展開する長方形）
	ResultFrame::Config frameConfig;
	frameConfig.center = frameCenter_;
	frameConfig.size = frameSize_;
	frameConfig.borderThickness = 6.0f;
	frameConfig.lineThickness = 10.0f;
	frameConfig.horizontalDuration = 0.5f;
	frameConfig.verticalDuration = 0.5f;
	frameConfig.borderColor = { 0.55f, 0.55f, 0.55f, 1.0f };
	frameConfig.innerColor = { 0.0f, 0.0f, 0.0f, 0.55f };
	frame_.Initialize(frameConfig);

	// 「CLEAR TIME」のラベル（左中央アンカーでスケール演出する）
	pClearTimeLabel_ = std::make_unique<Sprite>();
	pClearTimeLabel_->Initialize(Path::Image::ClearTime, { leftX_, labelCenterY_ }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.5f });

	const Vector2 naturalSize = pClearTimeLabel_->GetSize();
	labelFullSize_ = { naturalSize.x * (kLabelHeight_ / naturalSize.y), kLabelHeight_ };

	SpriteManager::GetInstance()->RegisterExternal(pClearTimeLabel_.get());

	// クリアタイムの数字
	clearTimeView_.Initialize();
	clearTimeView_.SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// ランキング
	this->InitializeRanking();
}

void ClearScene::InitializeRanking()
{
	RankingView::Config config;
	config.leftX = leftX_;
	config.slotCenterY = rankRowCenterY_;
	config.fontSize = kRankFontSize_;
	config.numberGap = kRankNumberGap_;
	config.rowSpacing = kRankRowSpacing_;
	rankingView_.Initialize(config);

	// 挿入前（＝現在保存されている）上位順位をまず表示しておく
	rankingView_.ShowStatic(oldRankingEntries_);
}

void ClearScene::StartRankingInsert()
{
	// 新記録（上位 kRankRowCount_ 位以内）に入ったときだけ、突き落とし挿入を再生する
	if (myRank_ >= 0 && !insertStarted_)
	{
		rankingView_.StartInsert(oldRankingEntries_, newRankingEntries_, myRank_);
		insertStarted_ = true;
	}
}

void ClearScene::UpdateCamera()
{
	/// ===================================================
	/// カメラ更新
	/// ===================================================
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	} else {
		vp_.UpdateMatrix();
	}
}

void ClearScene::Skip()
{
	frame_.Skip();
	revealTimer_ = kRevealDuration_;
	countTimer_ = kCountDuration_;
	phase_ = Phase::Done;

	// ランキングは最終状態（挿入後）へ飛ばす
	rankingView_.SkipToEnd(newRankingEntries_);
	insertStarted_ = true;
}

void ClearScene::ApplyReveal(float scale)
{
	// ラベル：左中央を固定したままスケールさせる
	pClearTimeLabel_->SetPosition({ leftX_, labelCenterY_ });
	pClearTimeLabel_->SetSize(labelFullSize_ * scale);

	// クリアタイム：左端と縦中心を固定したままフォントサイズをスケールさせる
	const float fontSize = timeFullFontSize_ * scale;
	clearTimeView_.SetFontSize(fontSize);
	clearTimeView_.SetOrigin({ leftX_, timeCenterY_ - fontSize * 0.5f });

	// ランキングも同じスケールで出現させる
	rankingView_.SetRevealScale(scale);
}

void ClearScene::ApplyCountUp(float seconds)
{
	clearTimeView_.SetSeconds(seconds);
}
