#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <debug/DebugEntry.h>
#include <functional>

class DebugEntryManager
{
public:

    static inline DebugEntryManager* GetInstance()
    {
        static DebugEntryManager instance;
        return &instance;
    }
    DebugEntryManager& operator=(const DebugEntryManager&) = delete;
    DebugEntryManager(const DebugEntryManager&) = delete;
    DebugEntryManager& operator=(DebugEntryManager&&) = delete;


    void Initialize();
    void Finalize();

    void RegisterEntry(const std::string& id, DebugEntry* pDebugEntry);
    void UnregisterEntry(DebugEntry* pDebugEntry);

    void ImGui();

    template <typename T>
    void HandleParameter(
        const std::string& id, 
        const std::string& name, 
        T* ptr, 
        std::function<void()> pFunc = nullptr);

    template <typename T>
    void HandleParameter(
        const std::string& id,
        const std::string& name,
        const T* ptr);

private:
    DebugEntryManager() = default;
    ~DebugEntryManager() = default;

    std::unordered_map<std::string, std::vector<DebugEntry*>> entries_;
};

template <typename T>
void DebugEntryManager::HandleParameter(const std::string& id, const std::string& name, T* ptr, std::function<void()> pFunc)
{
    entries_.at(id).back()->RegisterParameter(name, ptr, std::move(pFunc));
}

template <typename T>
void DebugEntryManager::HandleParameter(const std::string& id, const std::string& name, const T* ptr)
{
    entries_.at(id).back()->RegisterParameter(name, ptr);
}
