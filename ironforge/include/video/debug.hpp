#pragma once

#include <cstdint>
#include <string_view>

#include <renderer/renderer.hpp>

namespace video {
    struct instance_type;
    typedef instance_type instance_t;

    auto setup_debug() -> void;
    auto debug_text(video::instance_t &vi, std::unique_ptr<renderer::instance> &render, const float pos_x, const float pos_y, std::string_view text, uint32_t color) -> void;
} // namespace video
