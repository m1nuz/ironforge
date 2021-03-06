#pragma once

#include <memory>
#include <vector>
#include <core/math.hpp>
#include <video/texture.hpp>

namespace video {
    ///
    /// \brief The image_data struct
    ///
    struct image_data {
        image_data() = default;

        uint32_t                width = 0;
        uint32_t                height = 0;
        uint32_t                depth = 0;
        pixel_format            pixelformat = pixel_format::unknown;
        std::vector<uint8_t>    pixels;
    };

    namespace imgen {
        using rgb_color = glm::u8vec3;
        using rgba_color = glm::u8vec4;

        auto make_radial_gradient(int32_t width, int32_t height, uint8_t c0, uint8_t c1, int32_t radius) -> image_data;
        auto make_color(int32_t width, int32_t height, rgb_color color) -> image_data;
        auto make_color(int32_t width, int32_t height, rgba_color color) -> image_data;
        auto make_check(int32_t width, int32_t height, uint8_t mask, rgb_color color) -> image_data;
    } // namespace imgen
} // namespace video
