#pragma once

#include <cstdint>
#include <string>
#include <variant>


namespace ui {
    typedef uint32_t color_t;

    enum aligment : uint32_t {
        align_none              = 0,
        align_horizontal_center = 1 << 0,
        align_horizontal_left   = 1 << 1,
        align_horizontal_right  = 1 << 2,
        align_vertical_top      = 1 << 3,
        align_vertical_center   = 1 << 4,
        align_vertical_bottom   = 1 << 5,
        align_center            = align_horizontal_center | align_vertical_center,
    };

    namespace draw_commands {
        struct draw_line {
            draw_line() = default;

            float       x0 = 0.f;
            float       y0 = 0.f;
            float       x1 = 0.f;
            float       y1 = 0.f;
            float       w = 0.f;
            color_t     color = color_t{0};
        };

        struct draw_rect {
            draw_rect() = default;

            float       x = 0.f;
            float       y = 0.f;
            float       w = 0.f;
            float       h = 0.f;
            color_t     color = color_t{0};
        };

        struct draw_text {
            draw_text() = default;

            float       x = 0.f;
            float       y = 0.f;
            float       w = 0.f;
            float       h = 0.f;
            color_t     color = color_t{0};
            uint32_t    align = 0;
            size_t      pt_size = 0;
            uint32_t    font = static_cast<uint32_t>(-1);
            std::string text;
        };
    } // namespace draw_commands

    typedef std::variant<draw_commands::draw_line, draw_commands::draw_rect, draw_commands::draw_text> draw_command_t;
} // namespace ui
