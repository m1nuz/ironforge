#pragma once

#include <string>
#include <memory>
#include <vector>
#include <SDL2/SDL_events.h>
#include <ironforge_common.hpp>
#include <renderer/renderer.hpp>

namespace scene {
    enum class flags : uint32_t {
        start   = 0x00000001,
        current = 0x00000002
    };

    struct material_info {
        glm::vec3   emission;
        glm::vec3   ambient;
        glm::vec3   diffuse;
        glm::vec3   specular;
        float       shininess;
        float       transparency;

        const char  *emission_map;
        const char  *diffuse_map;
        const char  *specular_map;
        const char  *gloss_map;

        //texture     *emission_tex;
        //texture     *diffuse_tex;
        //texture     *specular_tex;
        //texture     *gloss_tex;

        const char  *name;
    };

    enum class mesh_source {
        file,
        gen_sphere,
        gen_cube,
        gen_grid
    };

    using material_instance = renderer::phong::material;

    struct mesh_info;
    // struct mesh_instance;
    struct model_info;
    struct model_instance;
    struct camera_info;
    struct camera_instance;
    struct script_info;
    struct script_instance;
    struct body_instance;
    struct body_info;
    struct transform_instance;
    struct entity_info;
    struct entity_instance;

    struct instance {
        instance();
        instance(const std::string& _name);
        ~instance();

        virtual auto append(const entity_info &info) -> void = 0;

        std::string     name;
        uint32_t        flags;
    };

    struct model_instance {

    };

    struct script_instance {

    };

    struct camera_info {
        float fov;
        float aspect;
        float znear;
        float zfar;
    };

    struct camera_instance {
        int         entity;
        int         parent;
        float       fov;
        float       aspect;
        float       znear;
        float       zfar;
        glm::mat4   projection;
        glm::mat4   view;
    };

    struct input_instance;

    struct input_action {
        const char *key_down;
        const char *key_up;

        SDL_Keycode key;
    };

    struct input_instance {

    };

    struct gen_cube_info {
        float size;
    };

    struct gen_sphere_info {
        int         rings;
        int         sectors;
        float       radius;
    };

    struct gen_grid_plane_info {
        float       horizontal_extend;
        float       vertical_extend;
        uint32_t    rows;
        uint32_t    columns;
        bool        triangle_strip;
    };

    struct mesh_info {
        mesh_source         source;
        const char          *filename;
        gen_cube_info       cube;
        gen_sphere_info     sphere;
        gen_grid_plane_info grid;
    };

    struct model_info {
        const char *name;
        std::vector<mesh_info> meshes;
    };

    struct body_info {
        glm::vec3 position;
        glm::vec3 orientation;
        glm::vec3 size;

        glm::vec3 velocity;
        glm::vec3 rotation;

        uint32_t  flags;
    };

    struct body_instance {

    };

    struct transform_instance {

    };

    struct script_info {
        const char  *name;
        const char  *source;
        const char  *class_name;
    };

    struct script {
        char        *name;
        char        *source;
        char        *table;
        int32_t     entity;
        uint32_t    flags;
    };

    struct entity {
        enum class flag {
            camera          = 0x00000001,
            current_camera  = 0x00000002,
            renderable      = 0x00000004,
            visible         = 0x00000008
        };
    };

    struct entity_info {
        const char          *name;
        body_info           *body;
        //point_light_info    *point_light;
        const char          *material;
        const char          *model;
        const char          *input;
        script_info         *script;
        //camera_info         *camera;
        int32_t             parent;
        uint32_t            flags;
    };

    auto load(const std::string& _name, uint32_t flags) -> std::unique_ptr<instance>;
    auto update(std::unique_ptr<instance>& s, float dt) -> void;
    auto present(std::unique_ptr<instance>& s, std::unique_ptr<renderer::instance> &render, float interpolation) -> void;
} // namespace scene
