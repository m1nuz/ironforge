#include <vector>
#include <algorithm>
#include <iterator>
#include <ironforge_common.hpp>
#include <ironforge_utility.hpp>
#include <core/application.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <renderer/renderer.hpp>
#include <ui/ui.hpp>

#include "timer.hpp"
#include "physics.hpp"
#include "transform.hpp"
#include "input.hpp"
#include "material.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "script.hpp"
#include "light.hpp"

#include <jansson.h>

#define json_error_if(obj, pred_fn, ret, root, ...) \
    if(pred_fn(obj)) { \
    application::error(application::log_category::game, __VA_ARGS__); \
    json_decref(root); \
    }

namespace scene {
    instance::instance() : instance{"empty", 0} {
    }

    instance::instance(const std::string& _name, uint32_t _state) : name{_name}, name_hash{utils::xxhash64(_name)}, state{_state} {
        application::debug(application::log_category::game, "Create '%' scene %\n", name, utils::to_hex(name_hash));
    }

    instance::~instance() {
        application::debug(application::log_category::game, "Destroy '%' scene\n", name);
    }

    struct simple_instance : public instance {
        simple_instance() : simple_instance("", 0) {

        }

        simple_instance(const std::string& _name, uint32_t _state)
            : instance{_name, _state}, size{0}, capacity{max_entities}, current_camera(0) {
            bodies.reserve(capacity);
            transforms.reserve(capacity);
            cameras.reserve(capacity);
            materials.reserve(capacity);
            inputs.reserve(capacity);
            models.reserve(capacity);
            scripts.reserve(capacity);
            ambient_lights.reserve(capacity);
            directional_lights.reserve(capacity);
            point_lights.reserve(capacity);
            names.reserve(capacity);
            name_hashes.reserve(capacity);
            flags.reserve(capacity);
        }

        ~simple_instance() {

        }

        virtual auto create_entity(const entity_info &info) -> int32_t {
            if ((size > capacity) || (size > INT_MAX)) {
                application::error(application::log_category::scene, "Can't create more then % entities\n", capacity);
                return -1;
            }

            int32_t eid = size++;

            auto parent_name = std::string{"root"};
            if (info.parent > 0)
                parent_name = names[info.parent];

            if (info.parent == eid)
                parent_name = "self";

            if (info.flags & static_cast<uint32_t>(entity_flags::current_camera))
                current_camera = eid;

            auto hash = utils::xxhash64(info.name, strlen(info.name), 0);

            // TODO: camera and no body? error
            // TODO: renderable and no body? error

            application::debug(application::log_category::scene, "Create entity '%' % parent '%' ID %\n", info.name, hash, parent_name, eid);

            if (info.flags & static_cast<uint32_t>(entity_flags::root)) {
                bodies.push_back(create_body(body_info{}));
                transforms.push_back(create_transform(0, 0));
                cameras.push_back(create_camera(eid, camera_info{}));
                materials.push_back(nullptr);
                inputs.push_back(nullptr);
                models.push_back(nullptr);
                ambient_lights.push_back(nullptr);
                directional_lights.push_back(nullptr);
                point_lights.push_back(nullptr);
                names.push_back(info.name);
                name_hashes.push_back(hash);
                flags.push_back(info.flags);
                scripts.push_back(info.script ? create_script(eid, *info.script, info.flags) : nullptr);
            } else {
                bodies.push_back(info.body ? create_body(*info.body) : nullptr);
                transforms.push_back(info.flags & static_cast<uint32_t>(entity_flags::renderable) ? create_transform(eid, info.parent) : nullptr);
                cameras.push_back(info.camera ? create_camera(eid, *info.camera) : nullptr);
                materials.push_back(info.material ? find_material(info.material) : nullptr);
                inputs.push_back(info.input ? create_input(eid, find_input_source(info.input)) : nullptr);
                models.push_back(info.model ? find_model(info.model) : nullptr);                

                auto any_light = info.light ? create_light(eid, *info.light) : std::make_pair(light_type::unknown, nullptr);
                switch (any_light.first) {
                case light_type::unknown:
                    ambient_lights.push_back(nullptr);
                    directional_lights.push_back(nullptr);
                    point_lights.push_back(nullptr);
                    break;
                case light_type::ambient:
                    ambient_lights.push_back(reinterpret_cast<ambient_light_instance*>(any_light.second));
                    directional_lights.push_back(nullptr);
                    point_lights.push_back(nullptr);
                    break;
                case light_type::directional:
                    ambient_lights.push_back(nullptr);
                    directional_lights.push_back(reinterpret_cast<directional_light_instance*>(any_light.second));
                    point_lights.push_back(nullptr);
                    break;
                case light_type::point:
                    ambient_lights.push_back(nullptr);
                    directional_lights.push_back(nullptr);
                    point_lights.push_back(reinterpret_cast<point_light_instance*>(any_light.second));
                    break;
                }

                names.push_back(info.name);
                name_hashes.push_back(hash);
                flags.push_back(info.flags);
                scripts.push_back(info.script ? create_script(eid, *info.script, info.flags) : nullptr);
            }            

            // TODO: check if all vectors is same size

            return eid;
        }

        virtual auto remove_entity(const int32_t id) -> bool {
            UNUSED(id);
            // TODO: implement
            return false;
        }

        virtual auto get_entity(const std::string &_name) -> int32_t {
            auto hash = utils::xxhash64(_name);

            // NOTE: find out what's better
            /*auto i = std::find(name_hashes.begin(), name_hashes.end(), hash);

            if (i != name_hashes.end())
                return std::distance(name_hashes.begin(), i);*/

            for (size_t i = 0; i < name_hashes.size(); ++i)
                if (name_hashes[i] == hash)
                    return i;

            return -1;
        }

        virtual auto get_entity_num() -> size_t {
            return size;
        }

        virtual auto get_transform(int32_t id) -> transform_instance* {
            assert(id >= 0);
            assert(id < (int32_t)transforms.capacity());

            return transforms[id];
        }

        virtual auto get_body(int32_t id) -> body_instance* {
            assert(id >= 0);
            assert(id < (int32_t)bodies.capacity());

            return bodies[id];
        }

        virtual auto get_material(int32_t id) -> material_instance* {
            assert(id >= 0);
            assert(id < (int32_t)transforms.capacity());

            return materials[id];
        }

        virtual auto get_model(int32_t id) -> model_instance* {
            assert(id >= 0);
            assert(id < (int32_t)models.capacity());

            return models[id];
        }

        virtual auto get_script(int32_t id) -> script_instance* {
            assert(id >= 0);
            assert(id < (int32_t)scripts.capacity());

            return scripts[id];
        }

        virtual auto get_current_camera() -> camera_instance* {
            assert(current_camera < cameras.capacity());

            return cameras[current_camera];
        }

        size_t                                      size;
        size_t                                      capacity;
        size_t                                      current_camera;

        // TODO: use handle for this
        std::vector<body_instance*>                 bodies;
        std::vector<transform_instance*>            transforms;
        std::vector<camera_instance*>               cameras;
        std::vector<material_instance*>             materials;
        std::vector<input_instance*>                inputs;
        std::vector<model_instance*>                models;
        std::vector<script_instance*>               scripts;
        std::vector<ambient_light_instance*>        ambient_lights;
        std::vector<directional_light_instance*>    directional_lights;
        std::vector<point_light_instance*>          point_lights;
        //std::vector<emitter_instance>               emitters;
        std::vector<std::string>                    names;
        std::vector<uint64_t>                       name_hashes;
        std::vector<uint32_t>                       flags;

        script_instance                             *main_script;
    };

    inline glm::vec3 json_vec3_value(json_t *arr) {
        if (json_array_size(arr) < 3) {
            application::warning(application::log_category::system, "%\n", "array size is not 3 elements");
            return {0.f, 0.f, 0.f};
        }

        auto value = glm::vec3{0.f, 0.f, 0.f};
        json_t *values[3];
        auto k = static_cast<size_t>(0);
        json_array_foreach(arr, k, values[k]) {
            if (k > 3)
                break;

            value[k] = json_real_value(values[k]);
        }

        return value;
    }

    auto init_all() -> void {
        init_all_timers();
        physics::init_all();
        init_all_transforms();
        init_all_materials();
        init_all_cameras();
        init_all_models();
        init_all_scripts();
    }

    auto cleanup_all() -> void {
        cleanup_all_timers();
        cleanup_all_scripts();
        cleanup_all_models();
        cleanup_all_cameras();
        cleanup_all_materials();
        cleanup_all_transforms();
        physics::cleanup();
    }

    auto empty(uint32_t state) -> std::unique_ptr<instance> {
        return make_unique<simple_instance>("empty", state);
    }

    auto load(const std::string& _name, uint32_t flags) -> std::unique_ptr<instance> {
        auto t = assets::get_text(_name);

        std::unique_ptr<instance> this_scene{new simple_instance(_name, flags)};

        // TODO: make error when t.text == null

        json_error_t error;
        auto root = json_loadb(t.text, t.size, 0, &error);
        json_error_if(root, !json_is_object, -1, root, "%\n", "root is not an object");

        auto scene_name = json_object_get(root, "name");
        json_error_if(scene_name, !json_is_string, -1, root, "%\n", "scene name is not a string");

        auto scene_version = json_object_get(root, "version");
        json_error_if(scene_version, !json_is_string, -1, root, "%\n", "scene version is not a string");

        // main script
        std::unique_ptr<script_info> main_si;
        auto main_script = json_object_get(root, "main_script");
        if (json_is_object(main_script)) {
            main_si.reset(new script_info);

            auto script_name = json_object_get(main_script, "name");
            json_error_if(script_name, !json_is_string, -1, root, "%s\n", "name is not string");

            auto source = json_object_get(main_script, "source");
            json_error_if(source, !json_is_string, -1, root, "%s\n", "source is not string");

            auto class_name = json_object_get(main_script, "class");
            json_error_if(class_name, !json_is_string, -1, root, "%s\n", "class is not string");

            main_si->name = json_string_value(script_name);
            main_si->source = json_string_value(source);
            main_si->table_name = json_string_value(class_name);
        }

        // create root object
        entity_info root_info;
        root_info.flags |= static_cast<uint32_t>(entity_flags::root);
        root_info.name = "root";
        root_info.parent = 0;
        if (main_si)
            root_info.script = main_si.get();
        this_scene->create_entity(root_info);

        // read effects
        auto effects = json_object_get(root, "effects");
        json_error_if(effects, !json_is_array, -1, root, "%\n", "effects is not an array");

        for(size_t i = 0; i < json_array_size(effects); i++) {
            auto effect = json_array_get(effects, i);
            json_error_if(effect, !json_is_object, -1, root, "%\n", "effect is not a object");

            auto name = json_object_get(effect, "name");
            json_error_if(name, !json_is_string, -1, root, "%\n", "name is not a string");

            auto type = json_object_get(effect, "type");
            json_error_if(type, !json_is_string, -1, root, "%\n", "type is not a string");

            application::debug(application::log_category::game, "Effect % '%'\n", json_string_value(type), json_string_value(name));

            if (strcmp(json_string_value(type), "skybox") == 0) {

                auto textures = json_object_get(effect, "textures");
                if (textures) {
                    std::string texs[6];

                    for (size_t j = 0; j < json_array_size(textures); j++) {
                        auto tex_name = json_array_get(textures, j);
                        texs[j] = json_string_value(tex_name);

                        application::debug(application::log_category::game, "TEX % %\n", json_string_value(name), json_string_value(tex_name));
                    }

                    // TODO: and add skybox texture to textures
                    video::make_texture_cube(json_string_value(name), texs);
                }
            }
        }

        // read materials
        // TODO: add materials sets
        auto materials = json_object_get(root, "materials");
        json_error_if(materials, !json_is_array, -1, root, "%\n", "materials is not an array");

        for(size_t i = 0; i < json_array_size(materials); i++) {
            auto material = json_array_get(materials, i);
            json_error_if(material, !json_is_object, -1, root, "%\n", "material is not an object");

            auto name = json_object_get(material, "name");
            json_error_if(name, !json_is_string, -1, root, "%\n", "name is not a string");

            auto type = json_object_get(material, "type");
            json_error_if(type, !json_is_string, -1, root, "%\n", "type is not a string");

            application::debug(application::log_category::game, "Material % %\n", json_string_value(type), json_string_value(name));

            if (strcmp(json_string_value(type), "phong") == 0) {
                auto ambient = json_object_get(material, "ambient");
                auto diffuse = json_object_get(material, "diffuse");
                auto specular = json_object_get(material, "specular");
                auto emission = json_object_get(material, "emission");
                auto shininess = json_object_get(material, "shininess");
                auto transparency = json_object_get(material, "transparency");
                auto reflectivity = json_object_get(material, "reflectivity");

                auto diffuse_map = json_object_get(material, "diffuse_map");
                auto specular_map = json_object_get(material, "specular_map");
                auto gloss_map = json_object_get(material, "gloss_map");
                auto emission_map = json_object_get(material, "emission_map");
                auto normal_map = json_object_get(material, "normal_map");

                material_info mi;
                memset(&mi, 0, sizeof mi);
                mi.name = json_string_value(name);

                if (json_is_array(ambient))
                    mi.ambient = json_vec3_value(ambient);

                if (json_is_array(diffuse))
                    mi.diffuse = json_vec3_value(diffuse);

                if (json_is_array(specular))
                    mi.specular = json_vec3_value(specular);

                if (json_is_array(emission))
                    mi.emission = json_vec3_value(emission);

                if (json_is_real(shininess))
                    mi.shininess = json_real_value(shininess);

                if (json_is_real(transparency))
                    mi.transparency = json_real_value(transparency);

                if (json_is_real(reflectivity))
                    mi.reflectivity = json_real_value(reflectivity);

                if (json_is_string(diffuse_map))
                    mi.diffuse_map = json_string_value(diffuse_map);

                if (json_is_string(specular_map))
                    mi.specular_map = json_string_value(specular_map);

                if (json_is_string(gloss_map))
                    mi.gloss_map = json_string_value(gloss_map);

                if (json_is_string(emission_map))
                    mi.emission_map = json_string_value(emission_map);

                if (json_is_string(normal_map))
                    mi.normal_map = json_string_value(normal_map);

                create_material(mi);
            }
        }

        // read models
        auto models = json_object_get(root, "models");
        json_error_if(models, !json_is_array, -1, root, "%\n", "models is not an array");

        for(size_t i = 0; i < json_array_size(models); i++) {
            auto model = json_array_get(models, i);
            json_error_if(model, !json_is_object, -1, root, "%\n", "model is not an object");

            auto name = json_object_get(model, "name");
            json_error_if(name, !json_is_string, -1, root, "%\n", "name is not a string");

            application::debug(application::log_category::game, "Model %\n", json_string_value(name));

            auto meshes = json_object_get(model, "meshes");
            json_error_if(meshes, !json_is_array, -1, root, "%\n", "meshes is not an array");

            model_info mi;
            mi.name = json_string_value(name);
            mi.meshes.resize(json_array_size(meshes));
            memset(&mi.meshes[0], 0, mi.meshes.capacity() * sizeof (mesh_info));

            for (size_t j = 0; j < json_array_size(meshes); j++) {
                auto mesh = json_array_get(meshes, j);
                json_error_if(mesh, !json_is_object, -1, root, "%\n", "mesh is not an object");

                auto type = json_object_get(mesh, "type");
                json_error_if(type, !json_is_string, -1, root, "%\n", "type is not a string");

                application::debug(application::log_category::game, "Mesh %\n", json_string_value(type));

                if (strcmp(json_string_value(type), "gen_sphere") == 0) {
                    auto radius = json_object_get(mesh, "radius");
                    json_error_if(radius, !json_is_real, -1, root, "%s\n", "radius is not a real");
                    auto rings = json_object_get(mesh, "rings");
                    json_error_if(rings, !json_is_integer, -1, root, "%s\n", "rings is not an int");
                    auto sectors = json_object_get(mesh, "sectors");
                    json_error_if(sectors, !json_is_integer, -1, root, "%s\n", "sectors is not an int");

                    mi.meshes[j].source = mesh_source::gen_sphere;
                    mi.meshes[j].sphere.radius = json_real_value(radius);
                    mi.meshes[j].sphere.rings = json_integer_value(rings);
                    mi.meshes[j].sphere.sectors = json_integer_value(sectors);
                }

                if (strcmp(json_string_value(type), "gen_cube") == 0) {
                    mi.meshes[j].source = mesh_source::gen_cube;
                }

                if (strcmp(json_string_value(type), "gen_plane") == 0) {
                    mi.meshes[j].source = mesh_source::gen_plane;
                }

                if (strcmp(json_string_value(type), "gen_grid") == 0) {
                    auto horizontal_extend = json_object_get(mesh, "horizontal_extend");
                    json_error_if(horizontal_extend, !json_is_number, -1, root, "%s\n", "horizontal_extend is not a number");

                    auto vertical_extend = json_object_get(mesh, "vertical_extend");
                    json_error_if(vertical_extend, !json_is_number, -1, root, "%s\n", "vertical_extend is not a number");

                    auto rows = json_object_get(mesh, "rows");
                    json_error_if(rows, !json_is_integer, -1, root, "%s\n", "rows is not an int");

                    auto columns = json_object_get(mesh, "columns");
                    json_error_if(columns, !json_is_integer, -1, root, "%s\n", "columns is not an int");

                    auto triangle_strip = json_object_get(mesh, "triangle_strip");

                    mi.meshes[j].source = mesh_source::gen_grid;
                    mi.meshes[j].grid.horizontal_extend = json_number_value(horizontal_extend);
                    mi.meshes[j].grid.vertical_extend = json_number_value(vertical_extend);
                    mi.meshes[j].grid.rows = json_integer_value(rows);
                    mi.meshes[j].grid.columns = json_integer_value(columns);
                    mi.meshes[j].grid.triangle_strip = json_is_true(triangle_strip);
                }
            }

            create_model(mi);
        }

        // read inputs
        auto inputs = json_object_get(root, "inputs");
        json_error_if(inputs, !json_is_array, -1, root, "%s\n", "inputs is not an array");

        for (size_t j = 0; j < json_array_size(inputs); j++) {
            auto input = json_array_get(inputs, j);
            json_error_if(input, !json_is_object, -1, root, "%s\n", "input is not an object");

            auto name = json_object_get(input, "name");
            json_error_if(name, !json_is_string, -1, root, "%s\n", "name is not a string");

            auto actions = json_object_get(input, "actions");
            json_error_if(actions, !json_is_array, -1, root, "%s\n", "actions is not an array");

            std::vector<input_action> input_actions;
            input_actions.resize(json_array_size(actions));

            for (auto i = static_cast<size_t>(0); i < json_array_size(actions); i++) {
                auto action = json_array_get(actions, i);
                json_error_if(action, !json_is_object, -1, root, "%s\n", "action is not an object");

                auto key = json_object_get(action, "key");
                if (json_is_string(key))
                    input_actions[i].key = SDL_GetKeyFromName(json_string_value(key));
                else
                    input_actions[i].key = SDLK_UNKNOWN;

                auto cbutton = json_object_get(action, "cbutton");
                if (json_is_string(cbutton))
                    input_actions[i].cbutton = SDL_GameControllerGetButtonFromString(json_string_value(cbutton));
                else
                    input_actions[i].cbutton = SDL_CONTROLLER_BUTTON_INVALID;

                auto caxis = json_object_get(action, "caxis");
                if (json_is_string(caxis))
                    input_actions[i].caxis = SDL_GameControllerGetAxisFromString(json_string_value(caxis));
                else
                    input_actions[i].caxis = SDL_CONTROLLER_AXIS_INVALID;

                auto on_keydown = json_object_get(action, "on_keydown");
                if (json_is_string(on_keydown))
                    input_actions[i].key_down = json_string_value(on_keydown);

                auto on_keyup = json_object_get(action, "on_keyup");
                if (json_is_string(on_keyup))
                    input_actions[i].key_up = json_string_value(on_keyup);

                auto on_cmotion = json_object_get(action, "cmotion");
                if (json_is_string(on_cmotion))
                    input_actions[i].caxis_motion = json_string_value(on_cmotion);
            }

            application::debug(application::log_category::game, "Input '%'\n", json_string_value(name));

            create_input_source(json_string_value(name), input_actions);
        }

        // read nodes
        auto nodes = json_object_get(root, "nodes");
        json_error_if(nodes, !json_is_array, -1, root, "%s\n", "nodes is not an array");

        for (size_t i = 0; i < json_array_size(nodes); i++) {
            auto node = json_array_get(nodes, i);
            json_error_if(node, !json_is_object, -1, root, "%s\n", "node is not an object");

            auto name = json_object_get(node, "name");
            json_error_if(name, !json_is_string, -1, root, "%s\n", "name is not a string");

            entity_info ei;
            //memset(&ei, 0, sizeof ei); NOTE: dont need it now

            ei.name = json_string_value(name);

            auto parent = json_object_get(node, "parent");
            if (json_is_string(parent))
                ei.parent = this_scene->get_entity(json_string_value(parent));

            // read node flags
            auto current_camera = json_object_get(node, "current_camera");
            if (json_is_true(current_camera))
                ei.flags |= static_cast<uint32_t>(entity_flags::current_camera);

            auto renderable = json_object_get(node, "renderable");
            if (json_is_true(renderable))
                ei.flags |= static_cast<uint32_t>(entity_flags::renderable);

            auto visible = json_object_get(node, "visible");
            if (json_is_true(visible))
                ei.flags |= static_cast<uint32_t>(entity_flags::visible);

            auto model = json_object_get(node, "model");
            if (json_is_string(model))
                ei.model = json_string_value(model);

            auto material_names = json_object_get(node, "materials");
            if (json_is_array(material_names)) {
                auto material_name = json_array_get(material_names, 0);
                ei.material = json_string_value(material_name);
            }

            std::unique_ptr<camera_info> ci{new camera_info};
            auto camera = json_object_get(node, "camera");
            if (json_is_object(camera)) {
                auto fov = json_object_get(camera, "fov");
                ci->fov = glm::radians(json_real_value(fov));

                auto znear =  json_object_get(camera, "znear");
                ci->znear = json_real_value(znear);

                auto zfar =  json_object_get(camera, "zfar");
                ci->zfar = json_real_value(zfar);

                ci->type = camera_type::perspective;
                ci->parent = ei.parent;
                ei.flags |= static_cast<uint32_t>(entity_flags::camera);
                ei.camera = ci.get();
            }

            std::unique_ptr<body_info> bi{new body_info};
            auto body = json_object_get(node, "body");
            if (json_is_object(body)) {
                memset(bi.get(), 0, sizeof (body_info));

                auto position = json_object_get(body, "position");
                if (json_is_array(position))
                    bi->position = json_vec3_value(position);

                auto size = json_object_get(body, "size");
                if (json_is_array(size))
                    bi->size = json_vec3_value(size);

                auto orientation = json_object_get(body, "orientation");
                if (json_is_array(orientation))
                    bi->orientation = glm::radians(json_vec3_value(orientation));

                auto velocity = json_object_get(body, "velocity");
                if (json_is_array(velocity))
                    bi->velocity = json_vec3_value(velocity);

                auto rotation = json_object_get(body, "rotation");
                if (json_is_array(rotation))
                    bi->rotation = json_vec3_value(rotation);

                ei.body = bi.get(); // create_body(bi)
            }

            light_info li;
            auto light = json_object_get(node, "light");
            if (json_is_object(light)) {
                auto type = json_object_get(light, "type");
                json_error_if(type, !json_is_string, -1, root, "%\n", "type is not a string");

                li.type = light_type::unknown;

                if (strcmp(json_string_value(type), "ambient_light") == 0) {
                    auto ambient = json_object_get(light, "ambient");
                    json_error_if(ambient, !json_is_array, -1, root, "%\n", "ambient is not an array");

                    auto La = json_vec3_value(ambient);

                    application::debug(application::log_category::game, "Ambient light % % %\n", La.x, La.y, La.z);

                    li.type = light_type::ambient;
                    li.ambient_light.ambient = La;
                }

                if (strcmp(json_string_value(type), "directional_light") == 0) {
                    auto diffuse = json_object_get(light, "diffuse");
                    json_error_if(diffuse, !json_is_array, -1, root, "%\n", "diffuse is not an array");
                    auto specular = json_object_get(light, "specular");
                    json_error_if(specular, !json_is_array, -1, root, "%\n", "specular is not an array");
                    auto direction = json_object_get(light, "direction");
                    json_error_if(direction, !json_is_array, -1, root, "%\n", "direction is not an array");

                    auto Ld = json_vec3_value(diffuse);
                    auto Ls = json_vec3_value(specular);
                    auto d = json_vec3_value(direction);

                    application::debug(application::log_category::game, "Directional light d% d% d% s% s% s% % % %\n",
                                       Ld.x, Ld.y, Ld.z, Ls.x, Ls.y, Ls.z, d.x, d.y, d.z);

                    li.type = light_type::directional;
                    li.directional_light.direction = d;
                    li.directional_light.diffuse = Ld;
                    li.directional_light.specular = Ls;
                }

                if (li.type != light_type::unknown)
                    ei.light = &li;
            }

            std::unique_ptr<script_info> si{new script_info};
            auto script = json_object_get(node, "script");
            if (json_is_object(script)) {
                auto script_name = json_object_get(script, "name");
                json_error_if(script_name, !json_is_string, -1, root, "%s\n", "name is not string");

                auto source = json_object_get(script, "source");
                json_error_if(source, !json_is_string, -1, root, "%s\n", "source is not string");

                auto class_name = json_object_get(script, "class");
                json_error_if(class_name, !json_is_string, -1, root, "%s\n", "class is not string");

                si->name = json_string_value(script_name);
                si->source = json_string_value(source);
                si->table_name = json_string_value(class_name);

                ei.script = si.get(); // create_script(si)
            }

            auto input = json_object_get(node, "input");
            if (json_is_string(input)) {
                ei.input = json_string_value(input);
            }

            this_scene->create_entity(ei);
        }

        application::debug(application::log_category::scene, "Scene entities %\n", this_scene->get_entity_num());

        // TODO: make optimization, sort all objets in arrays(material.cpp, ...)
        // to use binary search, make a flag that show then array is sorted, and we can
        // in other case use std::find

        return this_scene;
    }

    auto update(std::unique_ptr<instance>& s, float dt) -> void {
        UNUSED(s);
        physics::integrate_all(dt);
        scene::update_all_timers(dt);
        scene::update_all_scripts(dt);
    }

    auto process_event(std::unique_ptr<instance> &s, const SDL_Event &event) -> void {
        process_input_events(s, event);
    }

    auto present_all_transforms(std::unique_ptr<instance> &s, std::function<void(int32_t, const glm::mat4 &)> cb) -> void;
    auto present(std::unique_ptr<instance>& scn, std::unique_ptr<renderer::instance> &render, float interpolation) -> void {
        using namespace glm;

        process_all_materials();

        //application::debug(application::log_category::scene, "Present %\n", s->name);

        physics::interpolate_all(interpolation);
        scene::present_all_cameras(scn);
        scene::present_all_lights(scn, render);
        scene::present_all_transforms(scn, [&scn, &render](int32_t e, const glm::mat4 &m) {
            for (const auto &msh : scn->get_model(e)->meshes)
                for (const auto &dr : msh.draws) {
                    render->append(scn->get_material(e)->m0);
                    render->append(m); //render->append(scn->get_transform(e)->model);
                    render->append(msh.source, dr);
                }
        });

        static video::frame_stats stats;

        ui::command dt;
        dt.level = 0;
        dt.type = ui::command_type::text;
        dt.text.align = 0;
        dt.text.w = video::screen.width;
        dt.text.h = video::screen.height;
        dt.text.color = 0xffff00ff;
        dt.text.font = 0;
        dt.text.x = -0.98;
        dt.text.y = 0.92;
        dt.text.text = stats.info;
        dt.text.size = strlen(stats.info);

        video::begin(stats);
        render->dispath(dt);
        render->present(scn->get_current_camera()->projection, scn->get_current_camera()->view);
        video::end(stats);
    }
} // namespace scene


