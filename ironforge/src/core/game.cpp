#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <core/journal.hpp>
#include <core/json.hpp>
#include <core/assets.hpp>
#include <core/input.hpp>
#include <core/game.hpp>
#include <video/video.hpp>
#include <renderer/renderer.hpp>
#include <scene/scene.hpp>
#include <ui/ui.hpp>

#include "game_detail.hpp"

namespace game {

    static auto process_events(instance_t &app) -> void {
        SDL_Event ev = {};

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN)
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    app.running = false; // Exit when esc

            input::process_event(app, ev);
            //ui::process_event(in.uic, ev);
            scene::process_event(app.current_scene(), ev);
        }        
    }

    static auto cleanup_all(instance_t &app) -> void {
        app.render.reset(nullptr);

        video::cleanup(app.vi);
        scene::cleanup_all(app.scenes);
        assets::cleanup(app.asset_instance);
    }

    static auto update(instance_t &app, const float dt) -> void {
        input::update(app);
        assets::process(app.asset_instance);
        scene::update(app.current_scene(), dt);
        video::process(app.asset_instance, app.vi);
    }

    static auto present(instance_t &app, const float interpolation) -> void {
        using std::placeholders::_1;
        //ui::present(in.uic, std::bind(&renderer::instance::dispath, in.render.get(), _1));
        //scene::present(current_scene(), inst.render, interpolation);
        scene::present(app.vi, app.current_scene(), app.render, interpolation);
    }

    auto quit() -> void {
        journal::critical(journal::_GAME, "%", "Unexpected exit");
        exit(EXIT_SUCCESS);
    }

    static auto setup_locale(const char *locale) -> void {
        std::setlocale(LC_ALL, locale);
        std::locale::global(std::locale(locale));
    }

    auto create(std::string_view conf_path, const bool fullpath_only) -> game_result {
        using namespace std;

        // if releative and not fullpath_only add base_path
        const auto cpath = (conf_path.find("..") == std::string::npos) && !fullpath_only ? string{conf_path} : detail::get_base_path() + string{conf_path};
        auto contents = assets::get_config(cpath);
        if (!contents)
            return make_error_code(errc::init_conf);

        auto j = json::parse(contents.value());

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            return make_error_code(errc::init_platform);

        atexit(SDL_Quit);

        if (TTF_Init() < 0)
            return make_error_code(errc::init_platform);

        atexit(TTF_Quit);

        instance_t ctx;

        // Assets
        {
            auto asset_inst = assets::create_instance(assets::create_default_readers());
            if (holds_alternative<error_code>(asset_inst)) {
                journal::error(journal::_GAME, "%", "Can't append readers");
                return make_error_code(errc::init_assets);
            }

            ctx.asset_instance = move(get<assets::instance_t>(asset_inst));

            if (j.find("assets") == j.end())
                return make_error_code(errc::read_assets);

            for (auto &a : j["assets"]) {
                journal::debug(journal::_GAME, "%", a.get<string>());

                if (!assets::open(ctx.asset_instance, detail::get_base_path() + a.get<string>()))
                    return make_error_code(std::errc::io_error);
            }
        }

        // Video
        {
            if (j.find("video") == j.end())
                return make_error_code(errc::init_video);

            const auto video_info = j["video"];

            auto vc = video::init(ctx.asset_instance, video_info);
            if (!video::is_ok(vc))
                return get<error_code>(vc);

            ctx.vi = get<video::instance_t>(vc);

            journal::info(journal::_VIDEO, "%", video::get_info(ctx.vi));
        }

        // Scene
        {
            if (j.find("scenes") == j.end())
                return make_error_code(errc::read_scenes);

            string start_scene;

            if (j.find("start_scene") != j.end()) {
                start_scene = j["start_scene"].get<string>();
            }

            if (start_scene.empty())
                return make_error_code(errc::no_start_scene);

            scene::reset_scripts_engine();

            auto any_loaded = false;
            for (auto &sc : j["scenes"])
                if (start_scene == sc.get<string>()) {
                    auto res = scene::load(ctx.asset_instance, ctx.vi, start_scene);

                    if (!scene::is_ok(res))
                        return get<error_code>(res);

                    any_loaded = true;

                    ctx.scenes.push_back(get<scene::instance_t>(res));
                }

            scene::setup_bindings(ctx.current_scene());

            if (!any_loaded)
                return make_error_code(std::errc::io_error);
        }

        // Input
        if (!input::init(ctx))
            return make_error_code(errc::init_gamecontrollers);

        // Renderer
        {
            if (j.find("renderer") == j.end())
                return make_error_code(std::errc::io_error);

            const auto video_info = j["video"];
            const auto renderer_info = j["renderer"];
            const auto renderer_type = renderer_info.find("type") != renderer_info.end() ? renderer_info["type"].get<string>() : "null";

            ctx.render = renderer::create_renderer(renderer_type, ctx.vi, video_info);
        }

        // UI
        {
            const auto ui_info = j["ui"];
            const auto ui_type = ui_info.find("type") != ui_info.end() ? ui_info["type"].get<string>() : string{};

            if (ui_type == "immediate") {
                const auto styles = detail::read_ui_styles(ctx.asset_instance, ui_info);

                if (auto imui = imui::create(ctx.vi, styles); imui)
                    ctx.imui = imui.value();
            }
        }

        // Locale
        setup_locale("");

        return std::move(ctx);
    }

    auto launch(game::instance_t &app) -> int {
        using namespace std;

        /*auto loader = std::thread(assets::process_load, std::ref(app.asset_instance));

        assets::get_text(app.asset_instance, "gamecontrollerdb.txt", [] (const std::optional<assets::text_data_t> res) {
            if (res) {
                journal::info(journal::_GAME, "%", res.value());
            }
        });*/

        app.current_time = 0ull;
        app.last_time = 0ull;
        app.timesteps = 0ull;
        app.delta_accumulator = 0.0f;

        while (app.running) {
            process_events(app);

            app.last_time = app.current_time;
            app.current_time = SDL_GetPerformanceCounter();

            const auto freq = SDL_GetPerformanceFrequency();
            const auto dt = static_cast<float>(static_cast<double>(app.current_time - app.last_time) / static_cast<double>(freq));

            app.delta_accumulator += clamp(dt, 0.f, 0.2f);

            while (app.delta_accumulator >= timestep) {
                app.delta_accumulator -= timestep;

                update(app, timestep);

                app.timesteps++;
            }

            present(app, app.delta_accumulator / timestep);
        }

        cleanup_all(app);

        /*if (loader.joinable())
            loader.join();*/

        return EXIT_SUCCESS;
    }

} // namespace game
