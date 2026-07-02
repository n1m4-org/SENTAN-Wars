#pragma once
#include <memory>
#include "IWave.h"
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <functional>

enum class WaveType : uint32_t
{
    CommonWave = 0,
    BossWave = 1,
};

class WaveFactory
{
public:
    template <typename T>
    static constexpr void RegisterWave(WaveType type)
    {
        static_assert(std::is_base_of<IWave, T>::value, "T must be derived from IWave");
        waveCreators_[type] = []() -> std::unique_ptr<IWave>
        {
            return std::make_unique<T>();
        };
    }

    std::unique_ptr<IWave> CreateWave(uint32_t waveIndex);
private:
    static std::unordered_map<WaveType, std::function<std::unique_ptr<IWave>()>> waveCreators_;
};