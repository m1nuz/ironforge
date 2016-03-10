#include <vector>
#include <memory>
#include <algorithm>

#include <video/video.hpp>
#include <core/application.hpp>
#include <core/game.hpp>
#include <renderer/renderer.hpp>
#include <scene/scene.hpp>

namespace game {

    std::vector<std::unique_ptr<scene::instance>>   scenes;
    std::unique_ptr<renderer::instance>             render;

    static auto get_current() -> std::unique_ptr<scene::instance>& {
        // TODO: optimize this shit
        auto s = std::find_if(scenes.begin(), scenes.end(), [](std::unique_ptr<scene::instance>& sc) {
            if (sc->state & static_cast<uint32_t>(scene::state_flags::current))
                return true;
            return false;
        });

        if (s == scenes.end() && scenes.empty()) {
            scenes.push_back(scene::empty());
            return scenes.back();
        }

        /*for (size_t i = 0; i < scenes.size(); ++i)
            if (scenes[i]->state & static_cast<uint32_t>(scene::state_flags::current))
                return scenes[i];*/

        return *s;
    }

    __must_ckeck auto init(const std::string &title) -> result {
        auto video_result = video::result::failure;
        if ((video_result = video::init(title, 1280, 768, false)) != video::result::success) {
            application::error(application::log_category::video, "%\n", video::get_string(video_result));

            return game::result::error_init_video;
        }

        application::info(application::log_category::video, "%\n", video::get_info());

        //render = renderer::create_null_renderer();
        render = renderer::create_forward_renderer();

        scene::init_all(); // TODO: get result

        // TODO: call startup script here

        scenes.push_back(scene::load("scene01.scene", static_cast<uint32_t>(scene::state_flags::start) | static_cast<uint32_t>(scene::state_flags::current)));

        if (!render)
            return result::error_empty_render;

        if (scenes.empty())
            application::warning(application::log_category::game, "%\n", "No scene loaded");

        return result::success;
    }

    auto cleanup() -> void {
        scene::cleanup_all();

        for (auto& s : scenes)
            s.reset(nullptr);

        render.reset(nullptr);

        video::cleanup();
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
        case result::error_init_video:
            return "Can't init video";
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
