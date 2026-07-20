#pragma once
#include <span>
#include <Sprite.h>
#include <memory>

namespace utl::sprite
{
    void Unregister(std::span<Hagine::Sprite*> sprites);
    void Unregister(std::span<std::unique_ptr<Hagine::Sprite>> sprites);
}