#include <ironforge.hpp>
#include "config.hpp"

extern int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    using namespace application;

    result r;
    if ((r = init(GAME_NAME)) != result::success) {
        error(log_category::application, "%\n", get_string(r));
        return EXIT_FAILURE;
    }

    atexit(cleanup);

    if ((r = exec()) != result::success) {
        error(log_category::application, "%\n", "Can't execute application");
        return EXIT_FAILURE;
    }

    info(log_category::application, "%\n", get_string(r));

    return EXIT_SUCCESS;
}
