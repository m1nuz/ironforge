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

    enum class state_flags : uint32_t {
        start       = 0x00000001,
        current     = 0x00000002,
        empty       = 0x10000000,
    };

    inline state_flags operator |(state_flags a, state_flags b) {
        return static_cast<state_flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline state_flags operator |(uint32_t a, state_flags b) {
        return static_cast<state_flags>(a | static_cast<uint32_t>(b));
    }

    struct instance;

    auto init_all() -> void;
    auto cleanup_all() -> void;
    auto empty(uint32_t state = 0) -> std::unique_ptr<instance>;
    auto load(const std::string& _name, uint32_t flags) -> std::unique_ptr<instance>;
    auto update(std::unique_ptr<instance> &s, float dt) -> void;
    auto process_event(std::unique_ptr<instance> &s, const SDL_Event &event) -> void;
    auto present(std::unique_ptr<instance> &s, std::unique_ptr<renderer::instance> &render, float interpolation) -> void;
} // namespace scene
