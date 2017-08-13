#pragma once

#include <string>
#include <memory>
#include <SDL2/SDL_events.h>
#include <ironforge_common.hpp>
#include <renderer/renderer.hpp>

namespace scene {
    // limits
    constexpr size_t max_entities       = 100;
    constexpr size_t max_transforms     = 100;
    constexpr size_t max_bodies         = 100;
    constexpr size_t max_materials      = 100;
    constexpr size_t max_cameras        = 100;
    constexpr size_t max_points_lights  = 20;
    constexpr size_t max_scripts        = 20;
    constexpr size_t max_input_sources  = 20;
    constexpr size_t max_inputs         = 100;
    constexpr size_t max_timers         = 100;

    struct instance;

    auto init_all() -> void;
    auto cleanup_all() -> void;
    auto empty(const uint32_t state = 0) -> std::unique_ptr<instance>;
    auto load(const std::string& _name, uint32_t flags) -> std::unique_ptr<instance>;
    auto update(std::unique_ptr<instance> &s, const float dt) -> void;
    auto process_event(std::unique_ptr<instance> &s, const SDL_Event &event) -> void;
    auto present(std::unique_ptr<instance> &s, std::unique_ptr<renderer::instance> &render, const float interpolation) -> void;
} // namespace scene
