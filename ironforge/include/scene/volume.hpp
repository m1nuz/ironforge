#pragma once

#include <core/common.hpp>
#include <core/math.hpp>

namespace scene {
    struct bound_box { // AABB
        glm::vec3 min = glm::vec3(0.f);
        glm::vec3 max = glm::vec3(0.f);
    };

    struct bound_sphere {
        glm::vec3 center = glm::vec3(0.f);
        glm::vec3 radius = glm::vec3(0.f);
    };

    struct oriented_bound_box {

    };
} // namespace scene
