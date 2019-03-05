#pragma once

#include <core/json.hpp>

#include <optional>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
} // namespace assets

namespace video {
    struct instance_type;
    typedef instance_type instance_t;
} // namespace video

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;

    auto create_entity(assets::instance_t &asset, video::instance_t &vi, instance_t &sc, const json &info) -> uint32_t;

    auto find_entity_by_name(instance_t &sc, const std::string &name) -> uint32_t;

    auto remove_entity( instance_t &sc, const uint32_t entity_id ) -> bool;

    auto get_entity_name( instance_t &inst, const uint32_t entity_id ) -> std::optional<std::string_view>;
    auto get_entity_material( instance_t &inst, const uint32_t entity_id ) -> std::optional<material_ref>;
    auto get_entity_model( instance_t &inst, const uint32_t entity_id ) -> std::optional<model_ref>;
    auto get_entity_camera( instance_t &inst, const uint32_t entity_id ) -> std::optional<camera_ref>;
    auto get_entity_script( instance_t &inst, const uint32_t entity_id ) -> std::optional<script_ref>;
    auto get_entity_body( instance_t &inst, const uint32_t entity_id ) -> std::optional<body_ref>;
    auto get_entity_input( instance_t &inst, const uint32_t entity_id ) -> std::optional<input_ref>;
    auto get_entity_transform( instance_t &inst, const uint32_t entity_id ) -> std::optional<transform_ref>;
    auto get_entity_emitter( instance_t &inst, const uint32_t entity_id ) -> std::optional<emitter_ref>;
    auto get_entity_light( instance_t &inst, const uint32_t entity_id ) -> std::optional<light_ref>;

} // namespace scene
