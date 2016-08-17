#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ui {
    struct button;
    struct slider;
    struct list_box;

    struct text_box;

    struct label;
    struct progress_bar;

    struct window;

    struct context;
    struct renderer;

    enum widget_flags : uint32_t {
        wf_visible          = 0x0001,
        wf_noborder         = 0x0002,
        wf_nobackground     = 0x0004,
        wf_percent_values   = 0x0008
    };

    enum widget_state : uint32_t {
        ws_normal,
        ws_hovered,
        ws_pressed
    };

    struct button_info {
        float                   x, y, w, h;
        //float                   r;
        float                   border_width;
        uint32_t                align;
        int32_t                 font;
        size_t                  text_size;
        const char              *text;
        uint32_t                text_color;
        uint32_t                background_color;
        uint32_t                border_color;
        uint32_t                flags = wf_visible;
        int32_t                 level;
        int32_t                 style;

        std::function<void (int32_t)> on_click;
    };

    struct label_info {
        float                   x, y;
        //float                   r;
        float                   border_width;
        uint32_t                align;
        int32_t                 font;
        size_t                  text_size;
        const char              *text;
        uint32_t                text_color;
        uint32_t                background_color;
        uint32_t                border_color;
        uint32_t                flags = wf_visible | wf_noborder | wf_nobackground;
        int32_t                 level;
        int32_t                 style;
    };

    struct window_info {

    };

    auto create_button(button_info &info) -> int32_t;
    auto create_label(label_info &info) -> int32_t;
    auto create_window(window_info &info) -> int32_t;
} // namespace ui
