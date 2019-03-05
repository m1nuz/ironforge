#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

#include <core/json.hpp>

#include <SDL2/SDL_events.h>

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;

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
        uint32_t        entity;
        input_source    *source;
        std::vector<input_action> actions;
    };

    using input_ref = std::reference_wrapper<input_instance>;

    [[nodiscard]] auto create_input(const uint32_t entity, const json &info, const std::unordered_map<std::string, std::vector<input_action>> &sources) -> std::optional<input_instance>;

    auto create_input_source(scene::instance_t &s, const std::string &name, const std::vector<input_action> &actions) -> bool;
    auto process_input_events(scene::instance_t &s, const SDL_Event &e) -> void;
} // namespace scene
