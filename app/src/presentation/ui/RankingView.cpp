#include "RankingView.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <common/ResourcePath.h>
#include <Easing.h>
#include <SpriteManager.h>
#include <utility/SpriteUnregisterer.h>

using namespace Hagine;

namespace
{
	// 抑えめの金・銀・銅
	constexpr Vector4 kGold = { 0.83f, 0.69f, 0.33f, 1.0f };
	constexpr Vector4 kSilver = { 0.70f, 0.72f, 0.75f, 1.0f };
	constexpr Vector4 kBronze = { 0.74f, 0.53f, 0.36f, 1.0f };
	constexpr Vector4 kWhite = { 1.0f, 1.0f, 1.0f, 1.0f };

	/// <summary>Vector4 の線形補間</summary>
	Vector4 Lerp4(const Vector4& a, const Vector4& b, float t)
	{
		return {
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t,
			a.w + (b.w - a.w) * t,
		};
	}
}

void RankingView::Initialize(const Config& config)
{
	config_ = config;
	mode_ = Mode::Static;

	SpriteManager* sm = SpriteManager::GetInstance();

	// 順位番号（1, 2, 3）※左中央アンカーでスケール演出する
	for (size_t i = 0; i < kSlotCount_; ++i)
	{
		auto& sprite = numberSprites_[i];
		sprite = std::make_unique<Sprite>();
		sprite->Initialize(Path::Image::RankingNumbers[i + 1], { config_.leftX, config_.slotCenterY[i] }, MedalColor(i), { 0.0f, 0.5f });

		const Vector2 natural = sprite->GetSize();
		numberFullSize_[i] = { natural.x * (config_.fontSize / natural.y), config_.fontSize };
		sm->RegisterExternal(sprite.get());
	}

	// 順位番号の後のドット（"1." のように表示する）
	for (size_t i = 0; i < kSlotCount_; ++i)
	{
		auto& sprite = dotSprites_[i];
		sprite = std::make_unique<Sprite>();
		sprite->Initialize(Path::Image::Dot, { config_.leftX, config_.slotCenterY[i] }, MedalColor(i), { 0.0f, 0.5f });

		const Vector2 natural = sprite->GetSize();
		dotFullSize_ = { natural.x * (config_.fontSize / natural.y), config_.fontSize };
		sm->RegisterExternal(sprite.get());
	}

	// タイムの左端オフセット（全順位でタイムの列を揃えるため、最大の数字幅で計算する）
	// 「最大幅の数字 + ドット + 間隔」の右にタイムを置く
	numberAdvance_ = config_.fontSize + kNumberDotGap_ + dotFullSize_.x + config_.numberGap;

	// タイムの描画（最大 4 札）
	for (auto& view : cardViews_)
	{
		view.Initialize();
	}
}

void RankingView::Finalize()
{
	for (auto& view : cardViews_)
	{
		view.Finalize();
	}
	utl::sprite::Unregister(numberSprites_);
	utl::sprite::Unregister(dotSprites_);
}

void RankingView::Update(float deltaTime)
{
	if (mode_ == Mode::Sliding)
	{
		animTimer_ = (std::min)(animTimer_ + deltaTime, kSlideDuration_);
		const float t = ApplyEasing<float>(EasingType::OutCubic, 0.0f, 1.0f, animTimer_, kSlideDuration_);

		for (auto& card : cardData_)
		{
			if (!card.active)
			{
				continue;
			}
			card.currentY = card.startY + (card.endY - card.startY) * t;
			card.currentColor = Lerp4(card.startColor, card.endColor, t);
		}

		if (animTimer_ >= kSlideDuration_)
		{
			mode_ = (insertRank_ >= 0) ? Mode::Pulsing : Mode::Done;
			pulseTimer_ = 0.0f;
		}
	} else if (mode_ == Mode::Pulsing)
	{
		pulseTimer_ = (std::min)(pulseTimer_ + deltaTime, kPulseDuration_);
		if (pulseTimer_ >= kPulseDuration_)
		{
			mode_ = Mode::Done;
		}
	}

	this->LayoutFrame();
}

void RankingView::ShowStatic(const std::array<float, kRankCount>& entries)
{
	mode_ = Mode::Static;
	insertRank_ = -1;
	animTimer_ = 0.0f;
	pulseTimer_ = 0.0f;

	for (auto& card : cardData_)
	{
		card = Card{};
	}

	// 各順位スロットにそのまま配置する（札 i ↔ スロット i）
	for (size_t i = 0; i < kSlotCount_; ++i)
	{
		const bool valid = entries[i] > 0.0f;
		slotHasNumber_[i] = valid;
		if (!valid)
		{
			continue;
		}

		Card& card = cardData_[i];
		card.active = true;
		card.value = entries[i];
		card.startY = card.endY = card.currentY = config_.slotCenterY[i];
		card.startColor = card.endColor = card.currentColor = MedalColor(i);
		cardViews_[i].SetSeconds(entries[i]);
	}
}

void RankingView::StartInsert(const std::array<float, kRankCount>& oldEntries, const std::array<float, kRankCount>& newEntries, int insertRank)
{
	mode_ = Mode::Sliding;
	insertRank_ = insertRank;
	animTimer_ = 0.0f;
	pulseTimer_ = 0.0f;

	for (auto& card : cardData_)
	{
		card = Card{};
	}

	// 順位番号は挿入後の順位表に合わせて表示する
	for (size_t i = 0; i < kSlotCount_; ++i)
	{
		slotHasNumber_[i] = newEntries[i] > 0.0f;
	}

	size_t cardIndex = 0;

	// 既存の記録：挿入位置より下は 1 段下へ、あふれた分は落下させる
	for (size_t i = 0; i < kSlotCount_; ++i)
	{
		if (oldEntries[i] <= 0.0f)
		{
			continue;
		}

		Card& card = cardData_[cardIndex];
		card.active = true;
		card.value = oldEntries[i];

		if (static_cast<int>(i) < insertRank)
		{
			// 挿入位置より上：その場に留まる
			card.startY = card.endY = config_.slotCenterY[i];
			card.startColor = card.endColor = MedalColor(i);
		} else
		{
			const size_t dst = i + 1;
			if (dst < kSlotCount_)
			{
				// 1 段下へ突き落とされる
				card.startY = config_.slotCenterY[i];
				card.endY = config_.slotCenterY[dst];
				card.startColor = MedalColor(i);
				card.endColor = MedalColor(dst);
			} else
			{
				// ランキング外へ落下してフェードアウト
				card.startY = config_.slotCenterY[i];
				card.endY = config_.slotCenterY[i] + config_.rowSpacing;
				card.startColor = MedalColor(i);
				card.endColor = { MedalColor(i).x, MedalColor(i).y, MedalColor(i).z, 0.0f };
			}
		}

		card.currentY = card.startY;
		card.currentColor = card.startColor;
		cardViews_[cardIndex].SetSeconds(card.value);
		++cardIndex;
	}

	// 今回のタイム：挿入位置の 1 段上からフェードインしつつ落ちてくる
	if (insertRank >= 0 && cardIndex < kCardCount_)
	{
		Card& card = cardData_[cardIndex];
		card.active = true;
		card.inserted = true;
		card.value = newEntries[insertRank];
		card.startY = config_.slotCenterY[insertRank] - config_.rowSpacing;
		card.endY = config_.slotCenterY[insertRank];
		card.startColor = { MedalColor(insertRank).x, MedalColor(insertRank).y, MedalColor(insertRank).z, 0.0f };
		card.endColor = MedalColor(insertRank);
		card.currentY = card.startY;
		card.currentColor = card.startColor;
		cardViews_[cardIndex].SetSeconds(card.value);
	}
}

void RankingView::SkipToEnd(const std::array<float, kRankCount>& newEntries)
{
	this->ShowStatic(newEntries);
	mode_ = Mode::Done;
}

void RankingView::LayoutFrame()
{
	// パルス倍率（挿入行のみに適用）
	float pulseScale = 1.0f;
	if (mode_ == Mode::Pulsing)
	{
		const float pt = pulseTimer_ / kPulseDuration_;
		pulseScale = 1.0f + kPulseAmplitude_ * std::sin(std::numbers::pi_v<float> *pt);
	}

	const float timeLeftX = config_.leftX + numberAdvance_ * revealScale_;

	// 順位番号 ＋ ドット（"1." のように、ドットは番号の直後に置く）
	for (size_t i = 0; i < kSlotCount_; ++i)
	{
		auto& number = numberSprites_[i];
		auto& dot = dotSprites_[i];
		if (!slotHasNumber_[i])
		{
			number->SetSize({ 0.0f, 0.0f });
			dot->SetSize({ 0.0f, 0.0f });
			continue;
		}

		const float scale = revealScale_ * ((mode_ == Mode::Pulsing && static_cast<int>(i) == insertRank_) ? pulseScale : 1.0f);
		const Vector4& color = MedalColor(i);

		// 順位番号
		number->SetPosition({ config_.leftX, config_.slotCenterY[i] });
		number->SetSize(numberFullSize_[i] * scale);
		number->SetColor({ color.x, color.y, color.z });
		number->SetAlpha(color.w);

		// ドット（番号の実幅の直後に配置）
		const float dotX = config_.leftX + (numberFullSize_[i].x + kNumberDotGap_) * scale;
		dot->SetPosition({ dotX, config_.slotCenterY[i] });
		dot->SetSize(dotFullSize_ * scale);
		dot->SetColor({ color.x, color.y, color.z });
		dot->SetAlpha(color.w);
	}

	// タイム札
	for (size_t i = 0; i < kCardCount_; ++i)
	{
		auto& view = cardViews_[i];
		const Card& card = cardData_[i];

		if (!card.active)
		{
			view.SetFontSize(0.0f);
			view.SetOrigin({ timeLeftX, 0.0f });
			view.Update();
			continue;
		}

		const float scale = revealScale_ * ((mode_ == Mode::Pulsing && card.inserted) ? pulseScale : 1.0f);
		const float fontSize = config_.fontSize * scale;
		view.SetFontSize(fontSize);
		view.SetColor(card.currentColor);
		view.SetOrigin({ timeLeftX, card.currentY - fontSize * 0.5f });
		view.Update();
	}
}

const Vector4& RankingView::MedalColor(size_t rank) const
{
	switch (rank)
	{
	case 0:  return kGold;
	case 1:  return kSilver;
	case 2: return kBronze;
	default: return kWhite;
	}
}
