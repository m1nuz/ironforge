#pragma once

#include <cstddef>
#include <cstdint>

namespace ui {
    enum aligment : uint32_t {
        align_none              = 0,
        align_horizontal_center = 0x0001,
        align_horizontal_left   = 0x0002,
        align_horizontal_right  = 0x0004,
        align_vertical_top      = 0x0010,
        align_vertical_center   = 0x0020,
        align_vertical_bottom   = 0x0040,
    };

    enum class command_type : uint32_t {
        line,
        rect,
        rounded_rect,
        text,
        icon
    };

    struct draw_line_command {
        float       x0;
        float       y0;
        float       x1;
        float       y1;
        float       w;
        uint32_t    color;
    };

    struct draw_rect_command {
        float       x;
        float       y;
        float       w;
        float       h;
        uint32_t    color;
    };

    struct draw_round_rect_command {
        float       x;
        float       y;
        float       w;
        float       h;
        float       r;
        uint32_t    color;
    };

    struct draw_text_command {
        float       x;
        float       y;
        float       w;
        float       h;
        uint32_t    align;
        int32_t     font;
        const char  *text;
        size_t      size;
        uint32_t    color;
    };

    struct draw_icon_command {
        float       x;
        float       y;
        float       w;
        float       h;
        uint32_t    color;
        int32_t     icon;
    };

    struct command {
        command_type                type;
        int32_t                     level;

        union {
            draw_line_command       line;
            draw_rect_command       rect;
            draw_round_rect_command roundrect;
            draw_text_command       text;
            draw_icon_command       icon;
        };
    };

    inline bool operator < (const command &lhs, const command &rhs) {
        return lhs.level < rhs.level;
    }
} // namespace ui
