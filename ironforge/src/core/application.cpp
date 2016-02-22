#include <core/application.hpp>
#include <core/game.hpp>
#include <video/video.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <ironforge_common.hpp>

namespace application {

    log_priority categories[];
    const char *category_names[] = {
        "Application",
        "System",
        "Game",
        "Audio",
        "Video",
        "Render",
        "Input",
        "Scene"
    };

    _log_init_categories log_init_categories;
    static bool running = true;

    auto init(const std::string &title) -> result {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            return result::error_init_sdl;

        atexit(SDL_Quit);

        if (TTF_Init() < 0)
            return result::error_init_ttf;

        atexit(TTF_Quit);

        auto video_result = video::result::failure;
        if ((video_result = video::init(title, 1280, 768, false)) != video::result::success) {
            error(log_category::video, "%\n", video::get_string(video_result));

            return result::error_init_video;
        }

        info(log_category::video, "%\n", video::get_info());

        auto game_result = game::result::failure;
        if ((game_result = game::init()) != game::result::success) {
            error(log_category::game, "%\n", game::get_string(game_result));
            return result::error_init_game;
        }

        return result::success;
    }

    auto exec() -> result {
        SDL_Event e;

        auto current = 0ull;
        auto last = 0ull;
        auto timesteps = 0ull;
        auto accumulator = 0.0f;

        while (running) {
            while (SDL_PollEvent(&e))
                game::process_event(e);

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
            video::present();
        }

        return result::success;
    }

    auto quit() -> void {
        running = false;
    }

    auto cleanup() -> void {        
        game::cleanup();
        video::cleanup();

        info(log_category::application, "%\n", "Cleanup");
    }

    auto get_string(result r) -> const char * {
        switch (r) {
        case result::success:
            return "Success";
            break;
        case result::failure:
            return "Failure";
            break;
        case result::error_init_sdl:
            return SDL_GetError();
            break;
        case result::error_init_video:
            return "Can't init video";
            break;
        case result::error_init_game:
            return "Can't init game";
            break;
        default:
            return "Unknown error";
            break;
        }

        return nullptr;
    }
} // namespace application
