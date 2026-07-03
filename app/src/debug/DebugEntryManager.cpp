#include "DebugEntryManager.h"
#include <math/Color.h>
#include <utility/strutl.h>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG


void DebugEntryManager::Initialize()
{
    #ifdef _DEBUG
    ImGui::GetStyle().IndentSpacing = 16.0f;
    #endif // _DEBUG
}

void DebugEntryManager::Finalize()
{
}

void DebugEntryManager::RegisterEntry(const std::string& id, DebugEntry* pDebugEntry)
{
    entries_[id].push_back(pDebugEntry);
}

void DebugEntryManager::UnregisterEntry(DebugEntry* pDebugEntry)
{
    for (auto& [id, entryList] : entries_)
    {
        auto it = std::remove(entryList.begin(), entryList.end(), pDebugEntry);
        if (it != entryList.end())
        {
            entryList.erase(it, entryList.end());
            if (entryList.empty())
            {
                entries_.erase(id);
            }
            break;
        }
    }
}

void DebugEntryManager::ImGui()
{
    #ifdef _DEBUG

    bool isWindowOpen = ImGui::Begin("Debug Entries");

    if (!isWindowOpen)
    {
        ImGui::End();
        return;
    }

    for (auto& [id, entryList] : entries_)
    {
        bool isSoloEntry = entryList.size() == 1;

        /// カテゴリ名からハッシュ値を生成し、色相に変換してカテゴリごとに異なる色を割り当てる
        uint32_t hash = utl::string::to_hash(entryList.front()->GetCategory());
        HSV hsv = { static_cast<float>(hash % 360) / 360.0f, 0.65f, 0.85f };

        ImVec4 col = {};
        ImGui::ColorConvertHSVtoRGB(hsv.h(), hsv.s(), hsv.v(), col.x, col.y, col.z);
        col.w = 1.0f;
        ImGui::PushStyleColor(ImGuiCol_Text, col);

        bool isOpenParentTree = false;
        if (!isSoloEntry)
        {
            std::string label = entryList.front()->GetCategory() + " (" + std::to_string(entryList.size()) + ")";
            isOpenParentTree = ImGui::TreeNode(label.c_str());
        }

        if (isOpenParentTree || isSoloEntry)
        {
            for (size_t i = 0; i < entryList.size(); ++i)
            {
                auto& entry = entryList[i];
                std::string label = isSoloEntry ? entry->GetCategory() : entry->GetCategory() + " - " + std::to_string(i + 1);
                bool isOpen = ImGui::TreeNode(label.c_str());
                if (isOpen)
                {
                    ImGui::Indent();
                    entry->ImGui();
                    ImGui::Unindent();

                    ImGui::TreePop();
                }
            }
        }

        if (!isSoloEntry && isOpenParentTree)
        {
            ImGui::TreePop();
        }

        ImGui::PopStyleColor();
    }

    ImGui::End();

    #endif // _DEBUG
}
