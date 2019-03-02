#include <algorithm>
#include <iterator>

#include <core/common.hpp>
#include <core/json.hpp>
#include <utility/hash.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include <renderer/renderer.hpp>
#include <video/debug.hpp>

namespace scene {
    auto cleanup_all(std::vector<instance_t> &scenes) -> void {
        for (auto &s : scenes) {
            physics::cleanup_all(s);
        }
    }

    auto update(instance_t &sc, const float dt) -> void {
        physics::integrate_all(sc, dt);
        update_all_scripts(sc, dt);
        video::stats_update(dt);
    }

    auto process_event(instance_t &sc, const SDL_Event &ev) -> void {
        process_input_events(sc, ev);
    }

    auto present(video::instance_t &vi, instance_t &sc, std::unique_ptr<renderer::instance> &render, const float interpolation) -> void {
        using namespace glm;
        using namespace game;

        video::stats_clear();

        interpolate_all(sc, interpolation);
        scene::present_all_cameras(sc, vi.aspect_ratio);
        render->append(sc.skybox, renderer::SKYBOX_TEXTURE_BIT);
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

        video::stats::begin(vi.stats_info);
        video::debug_text(vi, render, -0.48, 0.42, vi.stats_info.info, 0x1a1a1aff);
        video::debug_text(vi, render, -0.48, 0.24, video::video_stats.info, 0x1a1a1aff);

        render->present(vi, sc.current_camera().projection, sc.current_camera().view);

        video::stats::end(vi.stats_info);
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


