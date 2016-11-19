#include <ironforge.hpp>
#include "config.hpp"

#include <cstdio>

extern int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    game::journal::add_storage(game::journal::console_storage);
    game::journal::add_storage(game::journal::file_storage);

    const auto start_script = game::get_base_path() + STARTUP_SCRIPT;

    return game::exec(start_script);
}
