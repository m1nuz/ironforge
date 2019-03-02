#pragma once

#include <vector>

#include <video/video.hpp>

namespace video {

    struct sprite_batch_info {
        uint32_t                max_sprites;
        texture                 tex;
    };

    struct sprite_batch {
        float                   correction;
        uint32_t                max_sprites;
        uint32_t                sprites_count;
        texture                 tex;

        std::vector<v3t2c4>     vertices;
        std::vector<uint16_t>   indices;

        vertices_source         source;
    };

    auto create_sprite_batch(instance_t &vi, const sprite_batch_info &info) -> sprite_batch;
    auto delete_sprite_batch(sprite_batch &sb) -> void;

    auto append_sprite_vertices(sprite_batch &sb, const v3t2c4 (&vertices)[4]) -> void;
    auto append_sprite(sprite_batch &sb, const glm::vec3 &position, const glm::vec2 size, const glm::vec4 offset, const glm::vec4 color) -> void;

    auto submit_sprite_batch(gl::command_buffer &cb, sprite_batch &sb, const gl::program &pm, const gl::sampler &sr) -> void;

} // namespace video
