#pragma once

#include <cstdint>
#include <ironforge_common.hpp>
#include <scene/scene.hpp>

namespace game {
    enum class result : int32_t {
        success,
        failure,
        error_init_video,
        error_empty_render,
    };

    constexpr auto timestep = 0.002f;

    auto quit() -> void;
    __must_ckeck auto current_scene() -> std::unique_ptr<scene::instance>&;
    __must_ckeck auto load_scene(const std::string &path) -> bool;
    __must_ckeck auto load_asset(const std::string &path) -> bool;
    __must_ckeck auto load_settings(const std::string &path) -> bool;

    auto get_base_path() -> const std::string&;
    auto get_pref_path() -> const std::string&;

    auto exec(const std::string &startup_script) -> int;
} // namespace game
