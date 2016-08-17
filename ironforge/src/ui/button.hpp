#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ui {
    struct button {
        int32_t                 id = 0;
        int32_t                 parent = 0;
        float                   x = 0, y = 0, w = 0, h = 0;
        //float                   r;
        float                   border_width = 0;
        uint32_t                align = 0;
        int32_t                 font = 0;
        size_t                  text_size = 0;
        const char              *text = nullptr;
        uint32_t                text_color = 0;
        uint32_t                background_color = 0;
        uint32_t                border_color = 0;
        uint32_t                flags = wf_visible;
        uint32_t                state = 0;
        int32_t                 level = 0;
        int32_t                 style = 0;

        std::function<void (int32_t)> on_click;
        std::function<void (int32_t)> on_hover;
    };
} // namespace ui
