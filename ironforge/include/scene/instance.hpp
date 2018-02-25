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

namespace scene {
    struct material_instance;
    struct mesh_instance;
    struct model_instance;
    struct camera_instance;
    struct script_instance;
    struct body_instance;
    struct input_instance;
    struct transform_instance;
    struct emitter_instance;

    struct bound_box; // AABB
    struct bound_sphere;
    struct oriented_bound_box;

    struct entity_info;

    struct instance {
        virtual ~instance() = default;

        virtual auto state() -> uint32_t = 0;

        virtual auto create_entity(const entity_info &info) -> int32_t = 0;
        virtual auto remove_entity(const int32_t id) -> bool = 0;
        virtual auto get_entity(const std::string &_name) -> int32_t = 0;
        virtual auto get_entity_num() -> size_t = 0;

        //virtual auto create_timer(const timer_info &info) -> int32_t = 0;

        // NOTE: return always valid and not null pointer
        virtual auto get_transform(int32_t id) -> transform_instance* = 0;
        virtual auto get_body(int32_t id) -> body_instance* = 0;
        virtual auto get_material(int32_t id) -> material_instance* = 0;
        virtual auto get_model(int32_t id) -> model_instance* = 0;
        virtual auto get_script(int32_t id) -> script_instance* = 0;
        //virtual auto get_timer(int32_t id) -> timer_instance* = 0;

        virtual auto get_current_camera() -> camera_instance* = 0;
    };

    struct emitter_instance {

    };

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
        typedef uint32_t index_t;
        typedef std::string name_t;
        typedef material_instance material_t;
        typedef mesh_instance mesh_t;
        typedef model_instance model_t;
        typedef camera_instance camera_t;
        typedef script_instance script_t;
        typedef body_instance body_t;
        typedef input_instance input_t;
        typedef transform_instance transform_t;
        typedef emitter_instance emitter_t;

        instance_type();

        // entity
        std::unordered_map<name_t, index_t>         names;
        std::unordered_map<index_t, material_t>     materials;
        std::unordered_map<index_t, mesh_t>         meshes;
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
    } instance_t;
} // namespace scene
