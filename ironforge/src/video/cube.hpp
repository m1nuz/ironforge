#pragma once

#include <cstdint>
#include <video/vertices.hpp>

namespace video {
    constexpr uint32_t cube_vertices_num = 24;
    constexpr uint32_t cube_indices_num = 36;
    constexpr uint32_t quad_vertices_num = 4;
    constexpr uint32_t quad_indices_num = 6;

    v3t2n3 cube_vertices_v3t2n3[cube_vertices_num] = {
        // front
        {{-1.f, 1.f, 1.f}, {0.f, 1.f}, { 0.f, 0.f, 1.f}},
        {{ 1.f, 1.f, 1.f}, {1.f, 1.f}, { 0.f, 0.f, 1.f}},
        {{ 1.f,-1.f, 1.f}, {1.f, 0.f}, { 0.f, 0.f, 1.f}},
        {{-1.f,-1.f, 1.f}, {0.f, 0.f}, { 0.f, 0.f, 1.f}},
        // back
        {{ 1.f, 1.f,-1.f}, {0.f, 1.f}, { 0.f, 0.f,-1.f}},
        {{-1.f, 1.f,-1.f}, {1.f, 1.f}, { 0.f, 0.f,-1.f}},
        {{-1.f,-1.f,-1.f}, {1.f, 0.f}, { 0.f, 0.f,-1.f}},
        {{ 1.f,-1.f,-1.f}, {0.f, 0.f}, { 0.f, 0.f,-1.f}},
        // top
        {{-1.f, 1.f,-1.f}, {0.f, 1.f}, { 0.f, 1.f, 0.f}},
        {{ 1.f, 1.f,-1.f}, {1.f, 1.f}, { 0.f, 1.f, 0.f}},
        {{ 1.f, 1.f, 1.f}, {1.f, 0.f}, { 0.f, 1.f, 0.f}},
        {{-1.f, 1.f, 1.f}, {0.f, 0.f}, { 0.f, 1.f, 0.f}},
        // bottom
        {{ 1.f,-1.f,-1.f}, {0.f, 1.f}, { 0.f,-1.f, 0.f}},
        {{-1.f,-1.f,-1.f}, {1.f, 1.f}, { 0.f,-1.f, 0.f}},
        {{-1.f,-1.f, 1.f}, {1.f, 0.f}, { 0.f,-1.f, 0.f}},
        {{ 1.f,-1.f, 1.f}, {0.f, 0.f}, { 0.f,-1.f, 0.f}},
        // left
        {{-1.f, 1.f,-1.f}, {0.f, 1.f}, {-1.f, 0.f, 0.f}},
        {{-1.f, 1.f, 1.f}, {1.f, 1.f}, {-1.f, 0.f, 0.f}},
        {{-1.f,-1.f, 1.f}, {1.f, 0.f}, {-1.f, 0.f, 0.f}},
        {{-1.f,-1.f,-1.f}, {0.f, 0.f}, {-1.f, 0.f, 0.f}},
        // right
        {{ 1.f, 1.f, 1.f}, {0.f, 1.f}, { 1.f, 0.f, 0.f}},
        {{ 1.f, 1.f,-1.f}, {1.f, 1.f}, { 1.f, 0.f, 0.f}},
        {{ 1.f,-1.f,-1.f}, {1.f, 0.f}, { 1.f, 0.f, 0.f}},
        {{ 1.f,-1.f, 1.f}, {0.f, 0.f}, { 1.f, 0.f, 0.f}}
    };

    uint16_t cube_indices_v3t2n3[cube_indices_num] = {
        0, 3, 1,  1, 3, 2,  // front
        4, 7, 5,  5, 7, 6,  // back
        8,11, 9,  9,11,10,  // top
        12,15,13, 13,15,14, // bottom
        16,19,17, 17,19,18, // left
        20,23,21, 21,23,22  // right
    };
} // namespace video
