#include <vector>
#include <memory>
#include <algorithm>

#include <core/application.hpp>
#include <core/game.hpp>
#include <renderer/renderer.hpp>
#include <scene/scene.hpp>

namespace game {

    std::vector<std::unique_ptr<scene::instance>>   scenes;
    std::unique_ptr<renderer::instance>             render;

    static auto get_current() -> std::unique_ptr<scene::instance>& {
        auto s = std::find_if(scenes.begin(), scenes.end(), [](std::unique_ptr<scene::instance>& sc) {
            if (sc->flags & static_cast<uint32_t>(scene::flags::current))
                return true;
            return false;
        });

        if (s == scenes.end() && scenes.empty()) {
            scenes.push_back(make_unique<scene::instance>());
            return scenes.back();
        }

        return *s;
    }

    __must_ckeck auto init() -> result {
        render = renderer::create_null_renderer();

        // TODO: call startup script here

        scenes.push_back(scene::load("scene01.scene", static_cast<uint32_t>(scene::flags::start)));

        if (!render)
            return result::error_empty_render;

        if (scenes.empty())
            application::warning(application::log_category::game, "%\n", "No scene loaded");

        return result::success;
    }

    auto cleanup() -> void {
        for (auto& s : scenes)
            s.reset(nullptr);

        render.reset(nullptr);
    }

    auto process_event(const SDL_Event &e) -> void {
        if (e.type == SDL_KEYDOWN)
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                application::quit();
            }

        // scene::process_event(get_current(), e);
    }

    auto update(float dt) -> void {
        scene::update(get_current(), dt);
    }

    auto present(float interpolation) -> void {
        scene::present(get_current(), render, interpolation);
    }

    auto get_string(result r) -> const char * {
        switch (r) {
        case result::success:
            return "Success";
            break;
        case result::failure:
            return "Failure";
            break;
        case result::error_empty_render:
            return "Empty render";
            break;
        default:
            return "Unknown error";
            break;
        }

        return nullptr;
    }
} // namespace game
