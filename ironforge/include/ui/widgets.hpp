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
    struct table;

    struct context;
    struct renderer;

    enum widget_type : uint32_t {
        wt_unknown,
        wt_button,
        wt_label
    };

    enum widget_flags : uint32_t {
        // for all widgets
        wf_visible          = 0x00000001,
        wf_noborder         = 0x00000002,
        wf_nobackground     = 0x00000004,
        wf_percent_values   = 0x00000008,
        // for containers
        wf_movable          = 0x00100000,
        wf_column           = 0x10000000,
        wf_row              = 0x20000000,
        wf_flexible         = 0x80000000
    };

    enum widget_state : uint32_t {
        ws_normal,
        ws_hovered,
        ws_pressed
    };

    struct button_info {
        float                   x, y, w, h;
        float                   translate_x = 0, translate_y = 0;
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
        int32_t                 level = 0;
        int32_t                 style;

        std::array<float, 4>    margins = {{0, 0, 0, 0}};

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
        float                   x, y, w, h;
        //float                   r;
        float                   border_width;
        int32_t                 font;
        float                   title_height;
        size_t                  title_size;
        const char              *title;
        uint32_t                text_color;
        uint32_t                background_color;
        uint32_t                border_color;
        uint32_t                flags = wf_visible;
        uint32_t                state;
        int32_t                 level = 0;
        int32_t                 style;

        std::array<float, 4>    padding = {{0, 0, 0, 0}};

        std::function<void (int32_t)> on_click;
        std::function<void (int32_t)> on_hover;
    };

    auto create_button(const button_info &info) -> int32_t;
    auto create_label(const label_info &info) -> int32_t;
    auto create_window(const window_info &info) -> int32_t;

    auto window_append(const int32_t win_id, const int32_t id) -> bool;
} // namespace ui
