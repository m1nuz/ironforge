#pragma once

#include <cstdint>
#include <array>

namespace ui {
    struct widget {
        int32_t                 id = 0;
        int32_t                 parent = 0;
        float                   x = 0;
        float                   y = 0;
        float                   w = 0;
        float                   h = 0;
        //float                   r = 0;
        float                   initial_x = 0;
        float                   initial_y = 0;
        float                   initial_w = 0;
        float                   initial_h = 0;
        float                   translate_x = 0;
        float                   translate_y = 0;

        std::array<float, 4>    padding = {{0, 0, 0, 0}};
        std::array<float, 4>    margins = {{0, 0, 0, 0}};

        uint32_t                flags = wf_visible;
    };
} // namespace ui
