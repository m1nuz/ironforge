#pragma once

#include <cstdint>

namespace video {
    struct _screen {
        int32_t     width;
        int32_t     height;
        float       aspect;
        int         msaa;
        bool        fullscreen;
        bool        vsync;
        bool        srgb_capable;
    };

    extern _screen screen;
} // namespace video
