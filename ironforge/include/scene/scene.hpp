#pragma once

#include <string>
#include <memory>
#include <vector>
#include <SDL2/SDL_events.h>
#include <ironforge_common.hpp>
#include <renderer/renderer.hpp>

namespace scene {
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

    enum class state_flags : uint32_t {
        start       = 0x00000001,
        current     = 0x00000002,
    };    

    struct material_info;
    struct material_instance;
    struct mesh_info;
    struct mesh_instance;
    struct model_info;
    struct model_instance;
    struct camera_info;
    struct camera_instance;
    struct script_info;
    struct script_instance;
    struct body_info;
    struct body_instance;
    struct input_instance;
    struct transform_instance;
    struct light_info;
    // struct any_light;
    struct emitter_info;
    struct emitter_instance;
    struct entity_info;

    struct bound_box; // AABB
    struct bound_sphere;
    struct oriented_bound_box;

    /*template <typename T>
    struct handle {
        int32_t id;
        T       *ptr;
    };*/

    struct instance {
        instance();
        instance(const std::string& _name, uint32_t _state);
        virtual ~instance();

        virtual auto create_entity(const entity_info &info) -> int32_t = 0;
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

    enum class entity_flags : uint32_t {
        root            = 0x00000001,
        camera          = 0x00000002,
        current_camera  = 0x00000004,
        renderable      = 0x00000008,
        visible         = 0x00000010
    };

    struct entity_info {
        const char          *name = nullptr;
        body_info           *body = nullptr;
        light_info          *light = nullptr;
        const char          *material = nullptr;
        const char          *model = nullptr;
        const char          *input = nullptr;
        script_info         *script = nullptr;
        emitter_info        *emitter = nullptr;
        camera_info         *camera = nullptr;
        int32_t             parent = 0;
        uint32_t            flags = 0;
    };

    auto init_all() -> void;
    auto cleanup_all() -> void;
    auto empty(uint32_t state = 0) -> std::unique_ptr<instance>;
    auto load(const std::string& _name, uint32_t flags) -> std::unique_ptr<instance>;
    auto update(std::unique_ptr<instance> &s, float dt) -> void;
    auto process_event(std::unique_ptr<instance> &s, const SDL_Event &event) -> void;
    auto present(std::unique_ptr<instance> &s, std::unique_ptr<renderer::instance> &render, float interpolation) -> void;
} // namespace scene
