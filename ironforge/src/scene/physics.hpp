#pragma once

#include <ironforge_common.hpp>

namespace physics {
    struct body_state {
        glm::vec3 position = glm::vec3(0.f);
        glm::vec3 orientation = glm::vec3(0.f);
        glm::vec3 size = glm::vec3(1.f);

        glm::vec3 velocity = glm::vec3(0.f);
        glm::vec3 rotation = glm::vec3(0.f);
    };

    auto init_all() -> void;
    auto integrate_all(float dt) -> void;
    auto interpolate_all(float interpolation) -> void;
} // namespace physics

namespace scene {
    struct body_info {
        glm::vec3 position = glm::vec3(0.f);
        glm::vec3 orientation = glm::vec3(0.f);
        glm::vec3 size = glm::vec3(1.f);

        glm::vec3 velocity = glm::vec3(0.f);
        glm::vec3 rotation = glm::vec3(0.f);

        uint32_t  flags = 0;
    };

    struct body_instance {
        physics::body_state current;
        physics::body_state previous;
        physics::body_state state;

        inline glm::vec3 position() {
            return current.position;
        }

        inline glm::vec3 orientation() {
            return current.orientation;
        }

        inline glm::vec3 size() {
            return current.size;
        }
    };

    auto create_body(const scene::body_info &info) -> body_instance*;
} // namespace scenes
