#pragma once
#include <string_view>


namespace ColliderTag
{
    enum class Type
    {
        None,
        Player,
        Enemy,
        PlayerAttack,
        EnemyAttack,
        WarpHole,
    };

    constexpr const char* ToTagName(Type type)
    {
        switch (type)
        {
        case Type::None:
            return "None";
        case Type::Player:
            return "Player";
        case Type::Enemy:
            return "Enemy";
        case Type::PlayerAttack:
            return "PlayerAttack";
        case Type::EnemyAttack:
            return "EnemyAttack";
        case Type::WarpHole:
            return "WarpHole";
        default:
            return "Unknown";
        }
    }
}