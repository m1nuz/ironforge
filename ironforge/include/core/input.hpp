#pragma once

#include <SDL2/SDL_events.h>

namespace game {

    struct instance_type;
    typedef instance_type instance_t;

    namespace input {
        auto init(instance_t &inst) -> bool;
        auto process_event(instance_t &inst, const SDL_Event &ev) -> void;
        auto update(instance_t &inst) -> void;
    } // namespace input

} // namespace game
