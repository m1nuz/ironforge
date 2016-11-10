#pragma once

#include <cstdint>
#include <video/vertices.hpp>

namespace video {
    constexpr uint32_t quad_vertices_num = 4;
    constexpr uint32_t quad_indices_num = 6;

    static v3t2n3 quad_vertices[quad_vertices_num] = {
        {{-1.f,  1.f, 0.f}, {0.f, 1.f}, {0.f, 0.f, 1.f}},
        {{ 1.f,  1.f, 0.f}, {1.f, 1.f}, {0.f, 0.f, 1.f}},
        {{ 1.f, -1.f, 0.f}, {1.f, 0.f}, {0.f, 0.f, 1.f}},
        {{-1.f, -1.f, 0.f}, {0.f, 0.f}, {0.f, 0.f, 1.f}},
    };

    static uint16_t quad_indices[quad_indices_num] = {
        0, 3, 1,  1, 3, 2
    };
} // namespace video
