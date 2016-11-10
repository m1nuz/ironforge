#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <SDL2/SDL_events.h>

namespace scene {
    struct input_action {
        std::string                 key_down;
        std::string                 key_up;
        std::string                 caxis_motion;

        SDL_Keycode                 key;
        SDL_GameControllerButton    cbutton;
        SDL_GameControllerAxis      caxis;
    };

    struct input_source {
        std::string                 name;
        uint64_t                    hash;
        std::vector<input_action>   actions;
    };

    struct input_instance {
        int32_t         entity;
        input_source    *source;
    };

    auto init_all_inputs() -> void;
    auto cleanup_all_inputs() -> void;

    auto create_input_source(const std::string &name, const std::vector<input_action> &actions) -> input_source*;
    auto find_input_source(const char *name) -> input_source*;

    auto create_input(int32_t entity, input_source *source) -> input_instance*;
    auto process_input_events(std::unique_ptr<instance> &s, const SDL_Event &e) -> void;
} // namespace scene
