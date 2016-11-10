#pragma once

#include <string>

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

    /*template <typename T>
    struct handle {
        int32_t id;
        T       *ptr;
    };*/

    struct entity_info;

    struct instance {
        instance();
        instance(const std::string& _name, uint32_t _state);
        virtual ~instance();

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

        std::string     name;
        uint64_t        name_hash;
        uint32_t        state;
    };
} // namespace scene
