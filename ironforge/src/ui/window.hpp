#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <functional>

#include "widget.hpp"

namespace ui {
    struct window : public widget {
        enum {
            wgf_sized                   = 0x0001
        };

        float                           border_width = 0;
        int32_t                         font = 0;
        float                           title_height = 0;
        size_t                          title_size = 0;
        const char                      *title = nullptr;
        uint32_t                        text_color = 0;
        uint32_t                        background_color = 0;
        uint32_t                        border_color = 0;
        uint32_t                        state = 0;
        int32_t                         level = 0;
        int32_t                         style = 0;

        std::vector<widget*>            widgets;
        std::vector<float>              grows;
        std::vector<uint32_t>           wflags;

        std::function<void (int32_t)>   on_click;
        std::function<void (int32_t)>   on_hover;
    };
} // namespace ui
