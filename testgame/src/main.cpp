#include <core/application.hpp>

#include "config.hpp"

extern int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    using namespace application;

    if (init(GAME_NAME) != result::success) {
        error(log_category::application, "%\n", "Can't init application");
        return EXIT_FAILURE;
    }

    atexit(cleanup);

    if (exec() != result::success) {
        error(log_category::application, "%\n", "Can't execute application");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
