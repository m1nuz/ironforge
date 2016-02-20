#include <core/application.hpp>
#include <video/video.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace application {

    log_priority categories[];
    const char *category_names[] = {
        "Application",
        "System",
        "Audio",
        "Video",
        "Render",
        "Input",
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

        video::result r;
        if ((r = video::init(title, 1280, 768, false)) != video::result::success) {
            error(log_category::video, "%\n", video::get_string(r));

            return result::error_init_video;
        }

        info(log_category::video, "%\n", video::get_info());

        return result::success;
    }

    auto exec() -> result {
        SDL_Event e;

        while (running) {
            while (SDL_PollEvent(&e)) {

            }

            if (e.type == SDL_KEYDOWN)
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }

            video::present();
        }

        return result::success;
    }

    auto cleanup() -> void {
        info(log_category::application, "%\n", "Cleanup");
        video::cleanup();
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
        default:
            return "unknown error";
            break;
        }

        return nullptr;
    }
} // namespace application
