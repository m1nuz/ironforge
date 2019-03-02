#pragma once

#include <memory>
#include <vector>
#include <optional>

#include <core/common.hpp>
#include <core/math.hpp>

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;

    struct transform_instance {
        uint32_t    entity = 0;
        uint32_t    parent = 0;
        glm::mat4   model = glm::mat4{1.f};
    };

    auto create_transforms(const uint32_t entity, const uint32_t parent) -> std::optional<transform_instance>;
    auto present_all_transforms(instance_t &sc, std::function<void(uint32_t, const glm::mat4 &)> cb) -> void;
} // namespace scene
