#include <ironforge.hpp>
#include "config.hpp"

#include <SDL2/SDL.h>

extern int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::string base_path = SDL_GetBasePath();
    base_path += "../../assets/forge";

    application::info(application::log_category::application, "BasePath %\n", base_path);

    auto asset_result = assets::result::failure;
    if ((asset_result = assets::append(assets::default_readers)) != assets::result::success) {
        application::error(application::log_category::application, "%\n", "Can't append readers");
        return EXIT_FAILURE;
    }

    if ((asset_result = assets::open(base_path)) != assets::result::success) {
        application::error(application::log_category::application, "%\n", "Can't open asset");
        return EXIT_FAILURE;
    }

    auto app_result = application::result::failure;
    if ((app_result = application::init(GAME_NAME)) != application::result::success) {
        application::error(application::log_category::application, "%\n", application::get_string(app_result));
        return EXIT_FAILURE;
    }

    atexit(application::cleanup);

    if ((app_result = application::exec()) != application::result::success) {
        application::error(application::log_category::application, "%\n", "Can't execute application");
        return EXIT_FAILURE;
    }

    application::info(application::log_category::application, "%\n", application::get_string(app_result));

    return EXIT_SUCCESS;
}
