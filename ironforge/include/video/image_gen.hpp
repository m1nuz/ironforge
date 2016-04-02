#pragma once

#include <memory>
#include <ironforge_common.hpp>
#include <video/texture.hpp>

namespace video {
    struct image_data {
        uint32_t        width;
        uint32_t        height;
        uint32_t        depth;
        pixel_format    pixelformat;
        uint8_t         *pixels; // TODO: make smart pointer
    };

    namespace imgen {
        using rgb_color = glm::u8vec3;

        auto make_radial_gradient(int32_t width, int32_t height, uint8_t c0, uint8_t c1, int32_t radius) -> image_data;
        auto make_color(int32_t width, int32_t height, rgb_color color) -> image_data;
        auto make_check(int32_t width, int32_t height, uint8_t mask, rgb_color color) -> image_data;
    } // namespace imggen
} // namespace video
