#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <video/video.hpp>
#include <core/application.hpp>
#include <core/journal.hpp>
#include <core/settings.hpp>
#include <core/assets.hpp>
#include <core/game.hpp>
#include <renderer/renderer.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include <ui/ui.hpp>

// FIXME: remove this
namespace scene {
    auto do_script(const std::string &name) -> bool;
} // namespace scene

namespace game {

    struct instance {
        auto get_current() -> std::unique_ptr<scene::instance>& {
            return scenes[current];
        }

        std::vector<std::unique_ptr<scene::instance>>   scenes;
        std::unique_ptr<renderer::instance>             render;
        std::unique_ptr<ui::context>                    uic;
        unsigned int                                    current = 0;
    };

    std::vector<std::unique_ptr<scene::instance>>   scenes;
    std::unique_ptr<renderer::instance>             render;
    std::unique_ptr<ui::context>                    uis;
    std::string some_text{"Lorem ipsum"};
    std::string some_text1{"AAA"};
    std::string some_text2{"BBB"};
    std::string some_text3{"CCC"};

    static bool running = true;

    auto quit() -> void {
        running = false;
    }

    auto get_current() -> std::unique_ptr<scene::instance>& {
        // TODO: optimize this shit
        auto s = std::find_if(scenes.begin(), scenes.end(), [](std::unique_ptr<scene::instance>& sc) {
            if (sc->state() & static_cast<uint32_t>(scene::state_flags::current))
                return true;
            return false;
        });

        if (s == scenes.end() && scenes.empty()) {
            scenes.push_back(scene::empty(static_cast<uint32_t>(scene::state_flags::current)));
            return scenes.back();
        }

        /*for (size_t i = 0; i < scenes.size(); ++i)
            if (scenes[i]->state & static_cast<uint32_t>(scene::state_flags::current))
                return scenes[i];*/

        return *s;
    }

    __must_ckeck auto load_scene(const std::string &path) -> bool {
        scenes.push_back(scene::load(path, static_cast<uint32_t>(scene::state_flags::start) | static_cast<uint32_t>(scene::state_flags::current)));
        return true;
    }

    __must_ckeck auto load_asset(const std::string &path) -> bool {
        auto asset_result = assets::result::failure;

        if ((asset_result = assets::open(game::get_base_path() + path)) != assets::result::success) {
            game::journal::error(game::journal::category::game, "%\n", "Can't open asset");
            return false;
        }

        return true;
    }

    __must_ckeck auto load_settings(const std::string &path) -> bool {
        if (!application::append_settings(game::get_base_path() + path))
        {
            game::journal::error(game::journal::category::game, "%\n", "Can't load settings");
            return false;
        }

        return true;
    }

    __must_ckeck auto init(const std::string &title, const std::string &startup_script) -> result {
        auto video_result = video::result::failure;
        if ((video_result = video::init(title, 1, 1, false, false, true)) != video::result::success) {
            game::journal::error(game::journal::category::video, "%\n", video::get_string(video_result));

            return game::result::error_init_video;
        }

        // TODO: call startup script here
        scene::init_all(); // TODO: get result
        scene::do_script(startup_script);

        const int w = application::int_value("video_width", 800);
        const int h = application::int_value("video_height", 600);
        const bool fullscreen = application::bool_value("video_fullscreen", false);
        const bool vsync = application::bool_value("video_vsync", false);

        if ((video_result = video::reset(w, h, fullscreen, vsync, true)) != video::result::success) {
            game::journal::error(game::journal::category::video, "%\n", video::get_string(video_result));

            return game::result::error_init_video;
        }

        game::journal::info(game::journal::category::video, "%\n", video::get_info());

        video::init_resources();

        //render = renderer::create_null_renderer();
        render = renderer::create_forward_renderer();        

        //scenes.push_back(scene::load("scene02.scene", static_cast<uint32_t>(scene::state_flags::start) | static_cast<uint32_t>(scene::state_flags::current)));

        if (!render)
            return result::error_empty_render;

        if (scenes.empty())
            game::journal::warning(game::journal::category::game, "%\n", "No scene loaded");


        auto controller_db = assets::get_text("gamecontrollerdb.txt");
        auto rw = SDL_RWFromMem(controller_db.text, controller_db.size);

        if (SDL_GameControllerAddMappingsFromRW(rw, 1) == -1)
            game::journal::error(game::journal::category::input, "%\n", SDL_GetError());

        uis = ui::create_context();
        /*ui::button_info bi;
        bi.w = 1.0;
        bi.h = 1.0;
        bi.x = 0.0;
        //bi.translate_x = -0.5;
        bi.y = 0.0;
        //bi.translate_y = -0.5;
        bi.text = some_text1.c_str();
        bi.text_size = some_text1.size();
        bi.font = 0;
        bi.align = ui::align_horizontal_center | ui::align_vertical_center;
        bi.text_color = 0xffffffff;
        bi.border_width = 0.002;
        bi.background_color = 0x111111ff;
        bi.border_color = 0x00ffffff;
        bi.level = 0;
        bi.margins = {0.025, 0.025, 0.025, 0.025};
        bi.on_click = [](int32_t id) {
            game::journal::info(game::journal::category::game, "Click %\n", id);
        };
        int btn1 = ui::create_button(bi);

        bi.y = 0.1;
        bi.text = some_text2.c_str();
        bi.text_size = some_text2.size();
        bi.level = 0;
        int btn2 = ui::create_button(bi);

        bi.y = 0.0;
        bi.text = some_text3.c_str();
        bi.text_size = some_text3.size();
        bi.level = 0;
        int btn3 = ui::create_button(bi);

        ui::window_info wi;
        wi.x = 0.0;
        wi.y = 0.0;
        wi.w = 0.4;
        wi.h = 0.4;
        wi.border_width = 0.002;
        wi.background_color = 0x333333ff;
        wi.border_color = 0x00ffffff;
        wi.level = 1;
        wi.flags = ui::wf_visible | ui::wf_movable | ui::wf_flexible | ui::wf_column;
        wi.padding = {0.025, 0.025, 0.025, 0.025};
        auto wnd = ui::create_window(wi);
        ui::window_append(wnd, btn1);
        ui::window_append(wnd, btn2);
        ui::window_append(wnd, btn3);*/

        return result::success;
    }

    auto cleanup() -> void {
        scene::cleanup_all();

        for (auto& s : scenes)
            s.reset(nullptr);

        render.reset(nullptr);

        video::cleanup();
    }

    static auto joystick_info(SDL_Joystick *joystick) -> void {
        char guid[64];
        SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guid, sizeof (guid));
        game::journal::info(game::journal::category::input, "          name: %\n", SDL_JoystickName(joystick));
        game::journal::info(game::journal::category::input, "          axes: %\n", SDL_JoystickNumAxes(joystick));
        game::journal::info(game::journal::category::input, "         balls: %\n", SDL_JoystickNumBalls(joystick));
        game::journal::info(game::journal::category::input, "          hats: %\n", SDL_JoystickNumHats(joystick));
        game::journal::info(game::journal::category::input, "       buttons: %\n", SDL_JoystickNumButtons(joystick));
        game::journal::info(game::journal::category::input, "   instance id: %\n", SDL_JoystickInstanceID(joystick));
        game::journal::info(game::journal::category::input, "          guid: %\n", guid);
        game::journal::info(game::journal::category::input, "gamecontroller: %\n", SDL_IsGameController(SDL_JoystickInstanceID(joystick)) ? "yes" : "no");
    }

    static auto controller_append(int dev) -> bool {
        if (SDL_IsGameController(dev)) {

            auto controller = SDL_GameControllerOpen(dev);
            if (!controller) {
                game::journal::error(game::journal::category::input, "Couldn't open controller %: %\n", dev, SDL_GetError());
                return false;
            }

            game::journal::info(game::journal::category::input, "Controller % opened\n", dev);

            joystick_info(SDL_GameControllerGetJoystick(controller));

            return true;
        }

        const char *name = SDL_JoystickNameForIndex(dev);
        game::journal::warning(game::journal::category::input, "Unknown controller %s\n", name ? name : "Unknown joystick");

        SDL_Joystick *joystick = SDL_JoystickOpen(dev);
        if (!joystick) {
            fprintf(stderr, "SDL_JoystickOpen(%d) failed: %s\n", dev, SDL_GetError());
            return false;
        }

        joystick_info(joystick);
        SDL_JoystickClose(joystick);

        return true;
    }

    auto process_event(const SDL_Event &e) -> void {
        if (e.type == SDL_KEYDOWN)
            if (e.key.keysym.sym == SDLK_ESCAPE)
                quit();

        switch (e.type) {
        case SDL_JOYDEVICEADDED:
            if (controller_append(e.jdevice.which))
                game::journal::info(game::journal::category::input, "Joystick device % added.\n", e.jdevice.which);
            break;
        case SDL_JOYDEVICEREMOVED:
            game::journal::info(game::journal::category::input, "Joystick device % removed.\n", e.jdevice.which);
            break;
        case SDL_CONTROLLERDEVICEADDED:
            game::journal::info(game::journal::category::input, "Controller device % added.\n", e.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            game::journal::info(game::journal::category::input, "Controller device % added.\n", e.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMAPPED:
            game::journal::info(game::journal::category::input, "Controller device % mapped.\n", e.cdevice.which);
            break;
        }

        ui::process_event(uis, e);
        scene::process_event(get_current(), e);
    }

    auto update(float dt) -> void {
        scene::update(get_current(), dt);
        video::process();

        SDL_JoystickUpdate();
        SDL_GameControllerUpdate();
    }

    auto present(float interpolation) -> void {
        using std::placeholders::_1;
        ui::present(uis, std::bind(&renderer::instance::dispath, render.get(), _1));
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
            game::journal::error(game::journal::category::game, "%\n", "Can't append readers");
            return EXIT_FAILURE;
        }

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            return EXIT_FAILURE;

        atexit(SDL_Quit);

        if (TTF_Init() < 0)
            return EXIT_FAILURE;

        atexit(TTF_Quit);

        auto game_result = game::result::failure;
        if ((game_result = game::init("IRON_FORGE", startup_script)) != game::result::success) {
            error(game::journal::category::game, "%\n", game::get_string(game_result));
            return EXIT_FAILURE;
        }

        SDL_Event e;

        auto current = 0ull;
        auto last = 0ull;
        auto timesteps = 0ull;
        auto accumulator = 0.0f;

        while (running) {
            while (SDL_PollEvent(&e))
                game::process_event(e);

            assets::process();

            last = current;
            current = SDL_GetPerformanceCounter();
            auto freq = SDL_GetPerformanceFrequency();

            auto dt = static_cast<float>(static_cast<double>(current - last) / static_cast<double>(freq));

            accumulator += glm::clamp(dt, 0.f, 0.2f);

            while (accumulator >= timestep) {
                accumulator -= timestep;

                game::update(timestep);

                timesteps++;
            }

            game::present(accumulator / timestep);
        }

        game::cleanup();

        return EXIT_SUCCESS;
    }
} // namespace game
