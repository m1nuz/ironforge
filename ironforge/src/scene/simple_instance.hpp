#pragma once

#include <core/journal.hpp>

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
#include "sprite.hpp"

namespace scene {
    struct simple_instance : public instance {
        simple_instance() {

        }

        simple_instance(const std::string& _name, uint32_t _state)
            : name{_name}, states{_state}, size{0}, capacity{max_entities}, current_camera(0) {
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

            game::journal::debug(game::journal::category::game, "Create '%' scene %\n", name);
        }

        ~simple_instance() {
            game::journal::debug(game::journal::category::game, "Destroy '%' scene\n", name);
        }

        virtual auto state() -> uint32_t {
            return states;
        }

        virtual auto create_entity(const entity_info &info) -> int32_t {
            if ((size > capacity) || (size > INT_MAX)) {
                game::journal::error(game::journal::category::scene, "Can't create more then % entities\n", capacity);
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

            game::journal::debug(game::journal::category::scene, "Create entity '%' % parent '%' ID %\n", info.name, hash, parent_name, eid);

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

        std::string                                 name;
        uint32_t                                    states;
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
        std::vector<sprite_instance*>               sprites;
        std::vector<std::string>                    names;
        std::vector<uint64_t>                       name_hashes;
        std::vector<uint32_t>                       flags;

        script_instance                             *main_script;
    };
} // namespace scene
