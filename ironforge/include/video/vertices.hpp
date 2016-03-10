#pragma once

#include <cstdint>
#include <ironforge_common.hpp>

namespace video {
    enum class vertex_attributes : uint32_t {
        position  = 0,
        texcoord  = 1,
        normal    = 2,
        color     = 4,
        tangent   = 5
    };

    enum class vertex_format : uint32_t {
        unknown,
        v3t2n3,
        v3t2c4
    };

    enum class index_format : uint32_t {
        unknown,
        ui16,
        ui32
    };

    struct v3t2n3 {
        glm::vec3 position;  // 12
        glm::vec2 texcoord;  // 8
        glm::vec3 normal;    // 12
    }; // 32b

    struct v3t2c4 {
        glm::vec3 position;  // 12
        glm::vec2 texcoord;  // 8
        glm::vec4 color;     // 16
    }; // 36b

    struct vertices_desc {
        uint32_t        primitive;
        vertex_format   vf;
        index_format    ef;
    };

    struct vertices_data {
        void        *vertices;
        void        *indices;
        uint32_t    vertices_num;
        uint32_t    indices_num;
    };

    struct vertices_info {
        vertices_data data;
        vertices_desc desc;
    };

    struct vertices_draw {
        uint32_t    vb_offset;
        uint32_t    ib_offset;
        uint32_t    count;
        uint32_t    base_vertex;
        uint32_t    base_index;
    };
} // namespace video
