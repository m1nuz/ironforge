#pragma once

#include <memory>
#include <vector>
#include <optional>

#include <core/common.hpp>

namespace scene {
    struct transform_instance {
        uint32_t     entity;
        uint32_t     parent;
        glm::mat4   model;
    };

    auto init_all_transforms() -> void;
    auto cleanup_all_transforms() -> void;
    auto create_transform(int32_t _entity, int32_t _parent) -> transform_instance*;

    auto create_transforms(const uint32_t entity, const uint32_t parent) -> std::optional<transform_instance>;
} // namespace scene
