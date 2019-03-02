#include <core/journal.hpp>
#include <scene/instance.hpp>
#include <scene/scene.hpp>

#include "entity.hpp"

namespace scene {
    auto create_entity(assets::instance_t &asset, video::instance_t &vi, instance_t &sc, const json &info) -> uint32_t {
        using namespace std;
        using namespace game;

        static instance_t::index_t entity_id = 0;

        string name;
        if (info.find("name") != info.end())
            name = info["name"].get<string>();

        const auto ix = (name != "root") ? entity_id++ : 0;

        if (!name.empty())
            sc.names.emplace(name, ix);

        const auto parent_name = info.find("parent") != info.end() ? info["parent"].get<string>() : string{};

        const auto parent_ix = find_entity_by_name(sc, parent_name);
        const auto renderable = info.find("renderable") != info.end() ? info["renderable"].get<bool>() : false;
        //const auto bool movable = info.find("movable") != info.end() ? info["movable"].get<bool>() : false;

        journal::info(journal::_SCENE, "Create entity:\n\tname '%' (%)\n\tparent '%' (%)\n\trenderable %", name, entity_id, parent_name, parent_ix, renderable);

        if (info.find("body") != info.end()) {
            const auto b = create_body(info["body"]);
            if (b)
                sc.bodies[ix] = b.value();
        }

        if (info.find("camera") != info.end()) {
            const auto c = create_camera(ix, info["camera"], vi.aspect_ratio);
            if (c) {
                sc.cameras[ix] = c.value();
                sc.current_camera_index = ix;
            }
        }

        if (renderable) {
            const auto t = create_transforms(ix, parent_ix);
            if (t)
                sc.transforms[ix] = t.value();
        }

        if (info.find("model") != info.end()) {
            const auto m = get_model(sc, info["model"].get<string>());
            if (m)
                sc.models[ix] = m.value();
        }

        if (info.find("light") != info.end()) {
            const auto l = create_light(info["light"]);
            if (l)
                sc.lights[ix] = l.value();
        }

        if (info.find("materials") != info.end()) {
            const auto mats = info["materials"];
            if (mats.size() != 0) {
                const auto mat_name = mats[0].get<string>();
                const auto m = get_material(sc, mat_name);
                if (m)
                    sc.materials[ix] = m.value();
            }
        }

        if (info.find("script") != info.end()) {
            const auto s = create_script(asset, ix, info["script"]);
            if (s)
                sc.scripts[ix] = s.value();
        }

        if (info.find("input") != info.end()) {
            const auto in = create_input(ix, info["input"], sc.input_sources);

            if (in)
                sc.inputs[ix] = in.value();
        }

        return ix;
    }

    // TODO: optional
    auto find_entity_by_name(instance_t &sc, const std::string &name) -> uint32_t {
        if (name.empty())
            return 0;

        auto it = sc.names.find(name);
        if (it != sc.names.end())
            return (*it).second;

        return 0;
    }

    auto remove_entity( instance_t &sc, const uint32_t entity_id ) -> bool {
        auto res = false;

        auto material_it = sc.materials.find( entity_id );
        if ( material_it != sc.materials.end() ) {
            res |= true;
            sc.materials.erase( material_it );
        }

        auto model_it = sc.models.find( entity_id );
        if ( model_it != sc.models.end() ) {
            res |= true;
            sc.models.erase( model_it );
        }

        auto camera_it = sc.cameras.find( entity_id );
        if ( camera_it != sc.cameras.end() ) {
            res |= true;
            sc.cameras.erase( camera_it );
        }

        auto script_it = sc.scripts.find( entity_id );
        if ( script_it != sc.scripts.end() ) {
            res |= true;
            sc.scripts.erase( script_it );
        }

        auto body_it = sc.bodies.find( entity_id );
        if ( body_it != sc.bodies.end() ) {
            res |= true;
            sc.bodies.erase( body_it );
        }

        auto input_it = sc.inputs.find( entity_id );
        if ( input_it != sc.inputs.end() ) {
            res |= true;
            sc.inputs.erase( input_it );
        }

        auto transform_it = sc.transforms.find( entity_id );
        if ( transform_it != sc.transforms.end() ) {
            res |= true;
            sc.transforms.erase( transform_it );
        }

        auto emitter_it = sc.emitters.find( entity_id );
        if ( emitter_it != sc.emitters.end() ) {
            res |= true;
            sc.emitters.erase( emitter_it );
        }

        auto light_it = sc.lights.find( entity_id );
        if ( light_it != sc.lights.end() ) {
            res |= true;
            sc.lights.erase( light_it );
        }

        return res;
    }
} // namespace scene
