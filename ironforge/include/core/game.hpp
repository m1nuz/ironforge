#pragma once

#include <cstdint>
#include <SDL2/SDL_events.h>
#include <ironforge_common.hpp>
#include <scene/scene.hpp>

namespace game {
    enum class result : int32_t {
        success,
        failure,
        error_init_video,
        error_empty_render,
    };

    auto get_current() -> std::unique_ptr<scene::instance>&;
    auto load_scene(const std::string &name) -> bool;
    __must_ckeck auto init(const std::string &title, const std::string &startup_script) -> result;
    auto cleanup() -> void;
    auto process_event(const SDL_Event &e) -> void;
    auto update(float dt) -> void;
    auto present(float interpolation) -> void;
    auto get_string(result r) -> const char *;
} // namespace game
