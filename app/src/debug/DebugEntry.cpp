#include "DebugEntry.h"

#include <debug/DebugEntryManager.h>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

DebugEntry::DebugEntry(const std::string& id, const std::string category)
{
    DebugEntryManager::GetInstance()->RegisterEntry(id, this);
    category_ = category;
}

DebugEntry::~DebugEntry()
{
    DebugEntryManager::GetInstance()->UnregisterEntry(this);
}

void DebugEntry::ImGui()
{
#ifdef _DEBUG

    for (auto& [name, func] : customGuiFunctions_)
    {
        // カスタム GUI 関数の呼び出し
        // BeginChild で子ウィンドウを作成し、その中でカスタム GUI を描画する
        ImGui::SeparatorText(name.c_str());
        ImGui::BeginChild(name.c_str(), ImVec2(0, 0), true);
        func();
        ImGui::EndChild();
    }

    for (auto& [name, data] : parameters_)
    {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int*>)
            {
                if (ImGui::DragInt(name.c_str(), arg))
                {
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, float*>)
            {
                if (ImGui::DragFloat(name.c_str(), arg, 0.001f))
                {
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, bool*>)
            {
                if (ImGui::Checkbox(name.c_str(), arg))
                {
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, std::string*>)
            {
                char buffer[256];
                strncpy_s(buffer, sizeof(buffer), arg->c_str(), _TRUNCATE);
                if (ImGui::InputText(name.c_str(), buffer, sizeof(buffer)))
                {
                    *arg = buffer;
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, Hagine::WorldTransform*>)
            {
                bool isChanged = false;
                isChanged |= ImGui::DragFloat3((name + " Scale").c_str(), &arg->scale_.x, 0.001f);
                isChanged |= ImGui::DragFloat3((name + " Rotate").c_str(), &arg->eulerRotation_.x, 0.001f);
                isChanged |= ImGui::DragFloat3((name + " Translate").c_str(), &arg->translation_.x, 0.001f);
                if (isChanged && data.onChange)
                {
                    data.onChange();
                }
            }
            else if constexpr (std::is_same_v<T, Hagine::Vector4*>)
            {
                if (ImGui::DragFloat4(name.c_str(), &arg->x, 0.001f))
                {
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, Hagine::Vector3*>)
            {
                if (ImGui::DragFloat3(name.c_str(), &arg->x, 0.001f))
                {
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, Hagine::Vector2*>)
            {
                if (ImGui::DragFloat2(name.c_str(), &arg->x, 0.001f))
                {
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            }
            else if constexpr (std::is_same_v<T, RGBA*>)
            {
                float color[4] = { arg->r / 255.0f, arg->g / 255.0f, arg->b / 255.0f, arg->a / 255.0f };
                if (ImGui::ColorEdit4(name.c_str(), color))
                {
                    arg->r = static_cast<uint8_t>(color[0] * 255);
                    arg->g = static_cast<uint8_t>(color[1] * 255);
                    arg->b = static_cast<uint8_t>(color[2] * 255);
                    arg->a = static_cast<uint8_t>(color[3] * 255);
                    if (data.onChange)
                    {
                        data.onChange();
                    }
                }
            } 
        }, data.ptr);
    }

    /// 定数パラメータの表示
    /// （定数パラメータは値の変更はできないが、値の確認はできるようにする）

    for (auto& [name, data] : parametersConstant_)
    {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, const int*>)
            {
                ImGui::Text("%s: %d", name.c_str(), *arg);
            }
            else if constexpr (std::is_same_v<T, const float*>)
            {
                ImGui::Text("%s: %.2f", name.c_str(), *arg);
            }
            else if constexpr (std::is_same_v<T, const bool*>)
            {
                ImGui::Text("%s: %s", name.c_str(), *arg ? "True" : "False");
            }
            else if constexpr (std::is_same_v<T, const std::string*>)
            {
                ImGui::Text("%s: %s", name.c_str(), arg->c_str());
            }
            else if constexpr (std::is_same_v<T, const Hagine::WorldTransform*>)
            {
                ImGui::Text("%s Scale: (%.2f, %.2f, %.2f)", name.c_str(), arg->scale_.x, arg->scale_.y, arg->scale_.z);
                ImGui::Text("%s Rotate: (%.2f, %.2f, %.2f)", name.c_str(), arg->eulerRotation_.x, arg->eulerRotation_.y, arg->eulerRotation_.z);
                ImGui::Text("%s Translate: (%.2f, %.2f, %.2f)", name.c_str(), arg->translation_.x, arg->translation_.y, arg->translation_.z);
            }
            else if constexpr (std::is_same_v<T, const Hagine::Vector4*>)
            {
                ImGui::Text("%s: (%.2f, %.2f, %.2f, %.2f)", name.c_str(), arg->x, arg->y, arg->z, arg->w);
            }
            else if constexpr (std::is_same_v<T, const Hagine::Vector3*>)
            {
                ImGui::Text("%s: (%.2f, %.2f, %.2f)", name.c_str(), arg->x, arg->y, arg->z);
            }
            else if constexpr (std::is_same_v<T, const Hagine::Vector2*>)
            {
                ImGui::Text("%s: (%.2f, %.2f)", name.c_str(), arg->x, arg->y);
            }
            else if constexpr (std::is_same_v<T, const RGBA*>)
            {
                ImGui::Text("%s: (R: %d, G: %d, B: %d, A: %d)", name.c_str(), arg->r, arg->g, arg->b, arg->a);
            }
        }, data);
    }

#endif // _DEBUG
}
