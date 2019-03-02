//#include <ironforge.hpp>

#include <core/journal.hpp>
#include <core/game.hpp>

extern int main(const int argc, const char* argv[]) {
    using namespace std;
    using namespace game;
    using namespace journal;

    // Get executable name
    const auto app_name = strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : nullptr;
    setup_default(string{app_name} + ".log");

    //set_verbosity(verbosity::error);
    //set_verbosity(_SCENE, verbosity::verbose);

    // Create game application
    auto res = create(argc, argv, "../assets/sandbox/game.conf");
    if (!is_ok(res)) {
        error(_GAME, "%", what(res));
        return EXIT_FAILURE;
    }

    auto app = move(get<instance_t>(res));

    // Launch the game
    return launch(app);
}
