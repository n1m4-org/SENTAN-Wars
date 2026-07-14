#include "SpriteFrameGroup.h"
#include <type/Vector2.h>
#include <algorithm>
#include <math/Mat3x3.h>
#include <random.h>
#include <SpriteManager.h>

using namespace Hagine;

#undef min
#undef max

SpriteFrameGroup::SpriteFrameGroup()
{
    #ifdef _DEBUG
    pPoint_ = std::make_unique<Hagine::Sprite>();
    float hueRandom = Hagine::Random::Range(0.0f, 1.0f);
    pPoint_->Initialize("debug/white1x1.png", {}, HSV(hueRandom, 0.45f, 0.85f).to_RGB().to_Vector4(0.5f));
    Hagine::SpriteManager::GetInstance()->RegisterExternal(pPoint_.get());

    auto pFunc = [this]()
    {
        if (ImGui::TreeNodeEx("Frame Property", ImGuiTreeNodeFlags_CollapsingHeader))
        {
            ImGui::DragFloat2("Position", &property_.standard.x, 0.1f);
        }

        if (entries_.empty())
        {
            ImGui::Text("No entries.");
            return;
        }

        std::string label = "Entries (" + std::to_string(entries_.size()) + ")";
        if (ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
        {
            for (size_t i = 0; i < entries_.size(); ++i)
            {
                auto& entry = entries_[i];
                std::string entryLabel = "Entry " + std::to_string(i) + ": " + entry.name;
                if (ImGui::TreeNodeEx(entryLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat2("Local Position", &entry.localPosition.x, 0.1f);
                    ImGui::SliderFloat2("Anchor Point", &entry.anchorPoint.x, 0.0f, 1.0f);

                    ImGui::TreePop();
                }
            }
        }
    };

    debugEntry.RegisterCustomGuiFunction("", pFunc);
    #endif // _DEBUG
}

void SpriteFrameGroup::DrawArea()
{
#ifdef _DEBUG
    pPoint_->Draw();
#endif // _DEBUG
}

void SpriteFrameGroup::ComputeAndApply()
{
    Ni::Matrix3x3 worldMatrix =
        Ni::Matrix3x3::ScaleMatrix({1.0f, 1.0f}) *
        Ni::Matrix3x3::RotateMatrix({}) *
        Ni::Matrix3x3::TranslateMatrix(property_.standard);

    for (auto& entry : entries_)
    {
        if (entry.sprite)
        {
            Vector2 spriteLeftTop = this->CalculateLeftTop(entry);
            // ワールド座標に変換
            Vector2 worldPosition = Ni::Transform(spriteLeftTop, worldMatrix);
            // スプライトの位置を更新
            entry.sprite->SetPosition(worldPosition);
        }
    }

#ifdef _DEBUG
    pPoint_->SetPosition(property_.standard);
#endif // _DEBUG
}

Hagine::Vector2 SpriteFrameGroup::CalculateLeftTop(const Entry& entry)
{
    Vector2 spriteSize = entry.sprite->GetSize();
    Vector2 anchorOffset = { spriteSize.x * entry.anchorPoint.x, spriteSize.y * entry.anchorPoint.y };
    return entry.localPosition - anchorOffset;
}
