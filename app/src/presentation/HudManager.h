#pragma once
#include <memory>


class HudManager
{
public:
    // HUDビューを更新する
    void Update()
    {
        for (auto& hudView : hudViews_)
        {
            hudView->Update();
        }
    }

    // HUDビューを生成する
    template <typename T, typename... Args>
    T* CreateView(Args&&... args)
    {
        auto hudView = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = hudView.get();
        hudViews_.emplace_back(std::move(hudView));
        return ptr;
    }

    void DestroyView(IHudView* ptr)
    {
        std::erase_if(hudViews_, [ptr](const std::unique_ptr<IHudView>& hudView)
        {
            return hudView.get() == ptr;
        });
    }

private:
    std::vector<std::unique_ptr<IHudView>> hudViews_;
};
