#include <vector>

#include "sprite.hpp"

namespace scene {
    std::vector<sprite_instance> sprites;

    auto create_sprite(const sprite_info &info) -> sprite_instance* {
        sprite_instance si;
        si.tex = info.tex;

        sprites.push_back(si);

        return &sprites.back();
    }
} // namespace scene

