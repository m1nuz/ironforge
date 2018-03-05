#include <vector>
#include <algorithm>
#include <iterator>
#include <core/common.hpp>
#include <utility/hash.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include <scene/entity.hpp>
#include <renderer/renderer.hpp>
#include <ui/ui.hpp>

#include "simple_instance.hpp"

#include <json.hpp>

namespace scene {
    auto reset_all(instance_t &sc) -> bool {
        (void)sc;
        //scene::reset_engine(sc);

        return true;
    }

    auto init_all() -> void {
        //init_all_timers();
        //physics::init_all();
        //init_all_transforms();
        //init_all_materials();
        //init_all_cameras();
        //init_all_models();
        //init_all_scripts();
    }

    auto cleanup_all() -> void {
        //cleanup_all_timers();
        //cleanup_all_scripts();
        //cleanup_all_models();
        //cleanup_all_cameras();
        //cleanup_all_materials();
        //cleanup_all_transforms();
        //physics::cleanup();
    }

    auto cleanup_all(std::vector<instance_t> &scenes) -> void {
        for (auto &s : scenes) {
            physics::cleanup_all(s);
            /*for (auto &t : s.transforms) {

            }*/
        }
    }

    auto update(instance_t &sc, const float dt) -> void {
        physics::integrate_all(sc, dt);
        update_all_scripts(sc, dt);
        // video::stats_update(dt);
    }

    auto process_event(instance_t &sc, const SDL_Event &ev) -> void {
        process_input_events(sc, ev);
    }

    auto present(video::instance_t &vi, instance_t &sc, std::unique_ptr<renderer::instance> &render, const float interpolation) -> void {
        using namespace glm;
        using namespace game;

        interpolate_all(sc, interpolation);
        scene::present_all_cameras(sc);
        render->append(sc.skybox, 0);
        scene::present_all_lights(sc, render);
        scene::present_all_transforms(sc, [&sc, &render] (uint32_t entity, const glm::mat4 &model) {
            const auto &mdl = sc.models[entity];

            for (const auto &msh : mdl.meshes) {
                const auto &mt = sc.materials[entity];

                render->append(mt.m0);
                render->append(model);
                render->append(msh.source, msh.draw);
            }
        });

        ui::command dt;
        dt.level = 0;
        dt.type = ui::command_type::text;
        dt.text.align = 0;
        dt.text.w = video::screen.width;
        dt.text.h = video::screen.height;
        dt.text.color = 0x1f1f1fff;
        dt.text.font = 0;
        dt.text.x = -0.98;
        dt.text.y = 0.92;
        dt.text.text = vi.stats_info.info;
        dt.text.size = vi.stats_info.info_size;

        ui::command dt2 = dt;
        dt2.text.y = 0.4;
        dt2.text.text = video::video_stats.info;
        dt2.text.size = video::video_stats.info_size;

        video::stats::begin(vi.stats_info);
        render->dispath(dt);
        render->dispath(dt2);
        render->present(vi, sc.current_camera().projection, sc.current_camera().view);
        video::stats::end(vi.stats_info);
    }

    auto create_entity(assets::instance_t &asset, instance_t &sc, const json &info) -> instance_t::index_t {
        using namespace std;
        using namespace game;

        static instance_t::index_t entity_id = 0;

        string name;
        if (info.find("name") != info.end())
            name = info["name"].get<string>();

        const auto ix = (name != "root") ? entity_id++ : 0;

        if (!name.empty())
            sc.names.emplace(name, ix);

        const string parent_name = info.find("parent") != info.end() ? info["parent"].get<string>() : string{};

        const auto parent_ix = find_entity(sc, parent_name);
        const auto renderable = info.find("renderable") != info.end() ? info["renderable"].get<bool>() : false;
        //const auto bool movable = info.find("movable") != info.end() ? info["movable"].get<bool>() : false;

        journal::info(journal::_SCENE, "Create entity:\n\tname '%' (%)\n\tparent '%' (%)\n\trenderable %", name, entity_id, parent_name, parent_ix, renderable);

        if (info.find("body") != info.end()) {
            const auto b = create_body(info["body"]);
            if (b)
                sc.bodies[ix] = b.value();
        }

        if (info.find("camera") != info.end()) {
            const auto c = create_camera(ix, info["camera"]);
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

    auto find_entity(instance_t &sc, const std::string &name) -> instance_t::index_t {
        if (name.empty())
            return 0;

        auto it = sc.names.find(name);
        if (it != sc.names.end())
            return (*it).second;

        return 0;
    }

    auto cache_model(instance_t &sc, const std::string &name, const model_instance &m) -> bool {
        if (sc.all_models.find(name) != sc.all_models.end())
            return false;

        sc.all_models.emplace(name, m);

        return true;
    }

    auto cache_material(instance_t &sc, const std::string &name, const material_instance &m) -> bool {
        if (sc.all_materials.find(name) != sc.all_materials.end())
            return false;

        sc.all_materials.emplace(name, m);

        return true;
    }

    auto get_model(instance_t &sc, const std::string &name) -> std::optional<model_instance> {
        auto it = sc.all_models.find(name);
        if (it == sc.all_models.end())
            return {};

        return (*it).second;
    }

    auto get_material(instance_t &sc, const std::string &name) -> std::optional<material_instance> {
        if (auto it = sc.all_materials.find(name); it != sc.all_materials.end())
            return (*it).second;

        return {};
    }
} // namespace scene


