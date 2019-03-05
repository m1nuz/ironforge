#pragma once

#include <optional>
#include <functional>

#include <core/common.hpp>
#include <core/json.hpp>
#include <core/math.hpp>

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;

    enum class camera_type : uint32_t {
        root,
        perspective
    };

    struct camera_instance {
        camera_instance() = default;

        uint32_t    entity = 0;
        uint32_t    parent = 0;
        camera_type type = camera_type::root;
        float       fov = 0.f;
        float       znear = 0.f;
        float       zfar = 0.f;
        glm::mat4   projection = glm::mat4{1.f};
        glm::mat4   view = glm::mat4{1.f};
    };

    using camera_ref = std::reference_wrapper<camera_instance>;

    auto create_camera(const uint32_t entity, const json &info, const float aspect_ratio) -> std::optional<camera_instance>;
    auto present_all_cameras(instance_t& sc, const float aspect_ratio) -> void;
} // namespace scene
