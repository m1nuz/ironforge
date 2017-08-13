#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <core/journal.hpp>
#include <core/assets.hpp>
#include <core/settings.hpp>
#include <core/game.hpp>
#include <video/video.hpp>
#include <renderer/renderer.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include <ui/ui.hpp>

#include "game_detail.hpp"

// FIXME: remove this
namespace scene {
    auto do_script(const std::string &name) -> bool;
} // namespace scene

namespace game {
    static instance                                 game_inst;

    auto quit() -> void {
        game_inst.running = false;
    }

    auto current_scene() -> std::unique_ptr<scene::instance>& {
        return game_inst.current_scene();
    }

    auto load_scene(const std::string &path) -> bool {
        game_inst.scenes.push_back(scene::load(path, 0));
        game_inst.current = game_inst.scenes.size() - 1;

        return true;
    }

    auto load_asset(const std::string &path) -> bool {
        if (assets::open(game::get_base_path() + path)!= assets::result::success)
            return false;

        return true;
    }

    auto load_settings(const std::string &path) -> bool {
        if (!application::append_settings(game::get_base_path() + path))
            return false;

        return true;
    }

    static __must_ckeck auto init(instance &inst, const std::string &title, const std::string &startup_script) -> result {
        srand(time(nullptr));

        auto video_result = video::result::failure;
        if ((video_result = video::init(inst.vi, title, 1, 1, false, false, true)) != video::result::success) {
            game::journal::error(game::journal::_VIDEO, "%", video::get_string(video_result));

            return game::result::error_init_video;
        }

        scene::init_all(); // TODO: get result
        scene::do_script(startup_script);

        const auto w = application::int_value("video_width", 800);
        const auto h = application::int_value("video_height", 600);
        const auto fullscreen = application::bool_value("video_fullscreen", false);
        const auto vsync = application::bool_value("video_vsync", false);


        if ((video_result = video::reset(inst.vi, w, h, fullscreen, vsync, true)) != video::result::success) {
            game::journal::error(game::journal::_VIDEO, "%", video::get_string(video_result));

            return game::result::error_init_video;
        }

        video::init_resources();

        game::journal::info(game::journal::_VIDEO, "%", video::get_info());

        //render = renderer::create_null_renderer();
        inst.render = renderer::create_forward_renderer();

        if (!inst.render)
            return result::error_empty_render;

        if (inst.scenes.empty())
            game::journal::warning(game::journal::_GAME, "%", "No scene loaded");


        auto controller_db = assets::get_text("gamecontrollerdb.txt");
        auto rw = SDL_RWFromMem(controller_db.text, controller_db.size);

        if (SDL_GameControllerAddMappingsFromRW(rw, 1) == -1)
            game::journal::error(game::journal::_INPUT, "%", SDL_GetError());

        game_inst.uic = ui::create_context();
        return result::success;
    }

    auto cleanup(instance &inst) -> void {
        scene::cleanup_all();

        for (auto& s : inst.scenes)
            s.reset(nullptr);

        inst.render.reset(nullptr);

        video::cleanup();
    }

    static auto joystick_info(SDL_Joystick *joystick) -> void {
        char guid[64];
        SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guid, sizeof (guid));
        game::journal::info(game::journal::_INPUT, "          name: %", SDL_JoystickName(joystick));
        game::journal::info(game::journal::_INPUT, "          axes: %", SDL_JoystickNumAxes(joystick));
        game::journal::info(game::journal::_INPUT, "         balls: %", SDL_JoystickNumBalls(joystick));
        game::journal::info(game::journal::_INPUT, "          hats: %", SDL_JoystickNumHats(joystick));
        game::journal::info(game::journal::_INPUT, "       buttons: %", SDL_JoystickNumButtons(joystick));
        game::journal::info(game::journal::_INPUT, "   instance id: %", SDL_JoystickInstanceID(joystick));
        game::journal::info(game::journal::_INPUT, "          guid: %", guid);
        game::journal::info(game::journal::_INPUT, "gamecontroller: %", SDL_IsGameController(SDL_JoystickInstanceID(joystick)) ? "yes" : "no");
    }

    static auto controller_append(int dev) -> bool {
        if (SDL_IsGameController(dev)) {

            auto controller = SDL_GameControllerOpen(dev);
            if (!controller) {
                game::journal::error(game::journal::_INPUT, "Couldn't open controller %: %", dev, SDL_GetError());
                return false;
            }

            game::journal::info(game::journal::_INPUT, "Controller % opened", dev);

            joystick_info(SDL_GameControllerGetJoystick(controller));

            return true;
        }

        const char *name = SDL_JoystickNameForIndex(dev);
        game::journal::warning(game::journal::_INPUT, "Unknown controller %", name ? name : "Unknown joystick");

        SDL_Joystick *joystick = SDL_JoystickOpen(dev);
        if (!joystick) {
            game::journal::error(game::journal::_INPUT, "SDL_JoystickOpen(%) failed: %", dev, SDL_GetError());
            return false;
        }

        joystick_info(joystick);
        SDL_JoystickClose(joystick);

        return true;
    }

    auto process_event(instance &inst, const SDL_Event &e) -> void {
        if (e.type == SDL_KEYDOWN)
            if (e.key.keysym.sym == SDLK_ESCAPE)
                quit();

        switch (e.type) {
        case SDL_JOYDEVICEADDED:
            if (controller_append(e.jdevice.which))
                game::journal::info(game::journal::_INPUT, "Joystick device % added.", e.jdevice.which);
            break;
        case SDL_JOYDEVICEREMOVED:
            game::journal::info(game::journal::_INPUT, "Joystick device % removed.", e.jdevice.which);
            break;
        case SDL_CONTROLLERDEVICEADDED:
            game::journal::info(game::journal::_INPUT, "Controller device % added.", e.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            game::journal::info(game::journal::_INPUT, "Controller device % added.", e.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMAPPED:
            game::journal::info(game::journal::_INPUT, "Controller device % mapped.", e.cdevice.which);
            break;
        }

        ui::process_event(inst.uic, e);
        scene::process_event(inst.current_scene(), e);
    }

    auto update(game::instance &inst, const float dt) -> void {
        scene::update(inst.current_scene(), dt);
        video::process();

        SDL_JoystickUpdate();
        SDL_GameControllerUpdate();
    }

    auto present(instance &inst, const float interpolation) -> void {
        using std::placeholders::_1;
        ui::present(inst.uic, std::bind(&renderer::instance::dispath, inst.render.get(), _1));
        scene::present(current_scene(), inst.render, interpolation);
    }

    constexpr auto get_string(const result r) -> const char * {
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

    auto get_base_path() -> const std::string& {
        static std::string base_path;
        if (!base_path.empty())
            return base_path;

        auto path = SDL_GetBasePath();
        base_path = path;
        SDL_free(path);

        return base_path;
    }

    auto get_pref_path() -> const std::string& {
        static std::string pref_path;

        if (!pref_path.empty())
            return pref_path;

        auto path = SDL_GetBasePath(); // TODO: change it to
        pref_path = path;
        SDL_free(path);

        return pref_path;
    }

    auto exec(const std::string &startup_script) -> int {
        auto asset_result = assets::result::failure;
        if ((asset_result = assets::append(assets::default_readers)) != assets::result::success) {
            game::journal::error(game::journal::_GAME, "%", "Can't append readers");
            return EXIT_FAILURE;
        }

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            return EXIT_FAILURE;

        atexit(SDL_Quit);

        if (TTF_Init() < 0)
            return EXIT_FAILURE;

        atexit(TTF_Quit);

        auto game_result = game::result::failure;
        if ((game_result = game::init(game_inst, "IRON_FORGE", startup_script)) != game::result::success) {
            journal::error(game::journal::_GAME, "%", game::get_string(game_result));
            return EXIT_FAILURE;
        }

        SDL_Event e;

        auto current = 0ull;
        auto last = 0ull;
        auto timesteps = 0ull;
        auto accumulator = 0.0f;

        while (game_inst.running) {
            while (SDL_PollEvent(&e))
                game::process_event(game_inst, e);

            assets::process();

            last = current;
            current = SDL_GetPerformanceCounter();
            const auto freq = SDL_GetPerformanceFrequency();

            const auto dt = static_cast<float>(static_cast<double>(current - last) / static_cast<double>(freq));

            accumulator += glm::clamp(dt, 0.f, 0.2f);

            while (accumulator >= timestep) {
                accumulator -= timestep;

                game::update(game_inst, timestep);

                timesteps++;
            }

            game::present(game_inst, accumulator / timestep);
        }

        game::cleanup(game_inst);

        return EXIT_SUCCESS;
    }
} // namespace game
