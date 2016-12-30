#include <ironforge.hpp>
#include "config.hpp"

#include <cstdio>
#include <functional>

extern int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    game::journal::add_tag(game::journal::_GAME, game::journal::verbosity::verbose);
    game::journal::add_tag(game::journal::_SYSTEM, game::journal::verbosity::verbose);
    game::journal::add_tag(game::journal::_AUDIO, game::journal::verbosity::verbose);
    game::journal::add_tag(game::journal::_VIDEO, game::journal::verbosity::verbose);
    game::journal::add_tag(game::journal::_RENDER, game::journal::verbosity::verbose);
    game::journal::add_tag(game::journal::_INPUT, game::journal::verbosity::verbose);
    game::journal::add_tag(game::journal::_SCENE, game::journal::verbosity::verbose);
    game::journal::add_tag(game::journal::_UI, game::journal::verbosity::verbose);

    game::journal::add_storage(game::journal::console_storage);

    using namespace std::placeholders;
    const std::string filelog = game::get_pref_path() + "journal.log";
    game::journal::add_storage(std::bind(game::journal::single_file_storage, filelog, _1, _2, _3, _4));

    std::unordered_map<std::string, std::string> tagged_files;
    tagged_files.emplace(game::journal::_GAME, game::get_pref_path() + "game.log");
    tagged_files.emplace(game::journal::_SYSTEM, game::get_pref_path() + "system.log");
    tagged_files.emplace(game::journal::_AUDIO, game::get_pref_path() + "audio.log");
    tagged_files.emplace(game::journal::_VIDEO, game::get_pref_path() + "video.log");
    tagged_files.emplace(game::journal::_RENDER, game::get_pref_path() + "render.log");
    tagged_files.emplace(game::journal::_INPUT, game::get_pref_path() + "input.log");
    tagged_files.emplace(game::journal::_SCENE, game::get_pref_path() + "scene.log");
    tagged_files.emplace(game::journal::_UI, game::get_pref_path() + "ui.log");
    game::journal::add_storage(std::bind(game::journal::multi_file_storage, tagged_files, _1, _2, _3, _4));

    const auto start_script = game::get_base_path() + STARTUP_SCRIPT;

    return game::exec(start_script);
}
