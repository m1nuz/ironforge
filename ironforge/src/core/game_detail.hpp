#pragma once

#include <string>
#include <SDL2/SDL_events.h>

namespace game {
    struct instance;
    enum class result;

    __must_ckeck auto init(instance &inst, const std::string &title, const std::string &startup_script) -> result;
    auto cleanup(game::instance &inst) -> void;
    auto process_event(game::instance &inst, const SDL_Event &e) -> void;
    auto update(game::instance &inst, const float dt) -> void;
    auto present(game::instance &inst, const float interpolation) -> void;
} // namespace game
