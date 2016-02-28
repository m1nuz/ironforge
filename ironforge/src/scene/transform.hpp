#pragma once

#include <memory>
#include <vector>
#include <ironforge_common.hpp>

namespace scene {
    struct transform_instance {
        int32_t     entity;
        int32_t     parent;
        glm::mat4   model;
    };

    auto init_all_transforms() -> void;
    auto create_transform(int32_t _entity, int32_t _parent) -> transform_instance*;
} // namespace scene
