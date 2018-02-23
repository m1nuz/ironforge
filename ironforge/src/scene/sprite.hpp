#pragma once

#include <core/common.hpp>
#include <video/video.hpp>
#include <scene/volume.hpp>

namespace scene {
    struct sprite_info {
        video::texture  tex;
        std::string     tex_name;
        glm::vec2       offset;
        glm::vec2       size;
    };

    struct sprite_instance {
        video::texture  tex;
        bound_box       aabb;
    };

    auto create_sprite(const sprite_info &info) -> sprite_instance*;
} // namespace scene
