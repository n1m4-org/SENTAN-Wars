#include "SpriteUnregisterer.h"
#include <SpriteManager.h>


void utl::sprite::Unregister(std::span<Hagine::Sprite*> sprites)
{
    for (auto sprite : sprites)
    {
        if (sprite)
        {
            Hagine::SpriteManager::GetInstance()->UnregisterExternal(sprite);
        }
    }
}

void utl::sprite::Unregister(std::span<std::unique_ptr<Hagine::Sprite>> sprites)
{
    for (auto& sprite : sprites)
    {
        if (sprite)
        {
            Hagine::SpriteManager::GetInstance()->UnregisterExternal(sprite.get());
        }
    }
}
