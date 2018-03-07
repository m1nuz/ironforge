#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

#include <video/glyphs.hpp>

#include "widget.hpp"

namespace ui {
    struct button : public widget {
        //float                   r;
        float                   border_width = 0;
        uint32_t                align = 0;
        video::font_t           *font = nullptr;
        size_t                  text_size = 0;
        const char              *text = nullptr;
        uint32_t                text_color = 0;
        uint32_t                background_color = 0;
        uint32_t                border_color = 0;
        uint32_t                state = 0;
        int32_t                 level = 0;
        int32_t                 style = 0;

        std::function<void (int32_t)> on_click;
        std::function<void (int32_t)> on_hover;
    };
} // namespace ui
