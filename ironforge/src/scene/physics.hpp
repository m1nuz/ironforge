#pragma once

#include <unordered_map>
#include <optional>

#include <core/common.hpp>
#include <core/json.hpp>
#include <core/math.hpp>

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace physics {
    struct body_state {
        glm::vec3 position = glm::vec3{0.f};
        glm::vec3 orientation = glm::vec3{0.f};
        glm::vec3 size = glm::vec3{1.f};

        glm::vec3 velocity = glm::vec3{0.f};
        glm::vec3 rotation = glm::vec3{0.f};
    };

    auto integrate_all(scene::instance_t &sc, const float dt) noexcept -> void;
    auto cleanup_all(scene::instance_t &sc) noexcept -> void;
} // namespace physics

namespace scene {
    struct body_info {
        glm::vec3 position = glm::vec3{0.f};
        glm::vec3 orientation = glm::vec3{0.f};
        glm::vec3 size = glm::vec3{1.f};

        glm::vec3 velocity = glm::vec3{0.f};
        glm::vec3 rotation = glm::vec3{0.f};

        uint32_t  flags = 0;
    };

    struct body_instance {
        body_instance() = default;

        physics::body_state current;
        physics::body_state previous;
        physics::body_state state;

        inline glm::vec3 position() const {
            return current.position;
        }

        inline glm::vec3 orientation() const {
            return current.orientation;
        }

        inline glm::vec3 size() const {
            return current.size;
        }
    };

    struct instance_type;
    typedef instance_type instance_t;

    auto create_body(const json &info) -> std::optional<body_instance>;
    auto interpolate_all(instance_t &sc, const float interpolation) -> void;
} // namespace scenes
