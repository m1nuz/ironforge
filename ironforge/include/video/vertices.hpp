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
} // namespace video
