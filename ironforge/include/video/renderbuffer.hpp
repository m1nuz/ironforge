#pragma once

#include <cstdint>
#include <video/texture.hpp>

namespace video {

    namespace gl330 {

        struct renderbuffer_info {
            pixel_format pf;
            uint32_t width;
            uint32_t height;
            uint32_t samples;
        };

        struct renderbuffer {
            uint32_t id;
            uint32_t width;
            uint32_t height;
            uint32_t samples;
        };

        auto create_renderbuffer(const renderbuffer_info &info) -> renderbuffer;
        auto destroy_renderbuffer(renderbuffer &buf) -> void;

    } // namespace gl330

} // namespace video
