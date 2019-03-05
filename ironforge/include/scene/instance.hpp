#pragma once

#include <string>
#include <unordered_map>

#include "../../src/scene/model.hpp"
#include "../../src/scene/material.hpp"
#include "../../src/scene/camera.hpp"
#include "../../src/scene/script.hpp"
#include "../../src/scene/physics.hpp"
#include "../../src/scene/input.hpp"
#include "../../src/scene/transform.hpp"
#include "../../src/scene/light.hpp"
#include "../../src/scene/timer.hpp"
#include "../../src/scene/emitter.hpp"
#include "../../src/scene/entity.hpp"

namespace scene {
    struct bound_box; // AABB
    struct bound_sphere;
    struct oriented_bound_box;

    // limits
    constexpr size_t max_entities       = 100;
    constexpr size_t max_transforms     = 100;
    constexpr size_t max_bodies         = 100;
    constexpr size_t max_materials      = 100;
    constexpr size_t max_cameras        = 100;
    constexpr size_t max_points_lights  = 20;
    constexpr size_t max_scripts        = 20;
    constexpr size_t max_input_sources  = 20;
    constexpr size_t max_inputs         = 100;
    constexpr size_t max_timers         = 100;

    constexpr size_t initial_name = 100;
    constexpr size_t initial_material = 100;
    constexpr size_t initial_mesh = 100;
    constexpr size_t initial_model = 100;
    constexpr size_t initial_camera = 10;
    constexpr size_t initial_script = 50;
    constexpr size_t initial_body = 100;
    constexpr size_t initial_input = 20;
    constexpr size_t initial_transform = 100;
    constexpr size_t initial_emitter = 20;
    constexpr size_t initial_light = 100;

    typedef struct instance_type {
        using index_t = uint32_t;
        using name_t = std::string;
        using material_t = material_instance ;
        using model_t = model_instance;
        using camera_t = camera_instance;
        using script_t = script_instance;
        using body_t = body_instance;
        using input_t = input_instance;
        using transform_t = transform_instance;
        using emitter_t = emitter_instance;
        using light_t = light_instance;

        instance_type();

        // entity
        std::unordered_map<name_t, index_t>         names;
        std::unordered_map<index_t, material_t>     materials;
        std::unordered_map<index_t, model_t>        models;
        std::unordered_map<index_t, camera_t>       cameras;
        std::unordered_map<index_t, script_t>       scripts;
        std::unordered_map<index_t, body_t>         bodies;
        std::unordered_map<index_t, input_t>        inputs;
        std::unordered_map<index_t, transform_t>    transforms;
        std::unordered_map<index_t, emitter_t>      emitters;
        std::unordered_map<index_t, light_t>        lights;

        std::unordered_map<std::string, std::vector<input_action>> input_sources;
        std::unordered_map<std::string, model_t>    all_models;
        std::unordered_map<std::string, material_t> all_materials;

        video::texture                              skybox;

        auto get_script(const uint32_t index) -> script_t& {
            return scripts[index];
        }

        auto get_body(const uint32_t index) -> body_t& {
            return bodies[index];
        }

        auto current_camera() -> camera_t&;

        index_t current_camera_index = 0;
        index_t current_entity_id = 1;
    } instance_t;
} // namespace scene
