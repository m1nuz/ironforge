#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <functional>

namespace ui {
    struct window {
        enum {
            wgf_sized = 0x0001
        };

        int32_t                 id = 0;
        int32_t                 parent = 0;
        float                   x = 0, y = 0, w = 0, h = 0;
        //float                   r;
        // top right bottom left
        std::array<float, 4>    padding = {{0, 0, 0, 0}};
        float                   border_width = 0;
        int32_t                 font = 0;
        float                   title_height = 0;
        size_t                  title_size = 0;
        const char              *title = nullptr;
        uint32_t                text_color = 0;
        uint32_t                background_color = 0;
        uint32_t                border_color = 0;
        uint32_t                flags = wf_visible;
        uint32_t                state = 0;
        int32_t                 level = 0;
        int32_t                 style = 0;

        std::vector<void*>      widgets;
        std::vector<float>      grows;
        std::vector<uint32_t>   wflags;
        std::vector<uint32_t>   types;

        std::function<void (int32_t)> on_click;
        std::function<void (int32_t)> on_hover;
    };
} // namespace ui
