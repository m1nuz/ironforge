#pragma once

#include <cstdint>
#include <vector>

namespace video {
    struct image_data;

    enum class pixel_format : uint32_t {
        unknown,
        r8,
        rg8,
        rgb8,
        rgba8,
        bgr8,
        bgra8,
        r16f,
        r32f,
        rgb16f,
        rgba16f,
        rgb32f,
        rgba32f,
        depth
    };

    enum class texture_flags : uint32_t {
        // data = 0x00000001,
        mipmaps         = 0x00000002,
        auto_mipmaps    = 0x00000004,
    };

    struct texture_info {
        texture_info() = default;
        /*texture_info(pixel_format _format, int32_t _mipmaps, uint32_t _flags, int32_t w, int32_t h, int32_t d, void *_pixels)
            : format{_format}, mipmaps{_mipmaps}, flags{_flags}, width{w}, height{h}, depth{d}, pixels{_pixels} {
        }*/

        pixel_format format = pixel_format::unknown;
        int32_t     mipmaps = 0;
        uint32_t    flags = 0;

        uint32_t    width = 0;
        uint32_t    height = 0;
        uint32_t    depth = 0;

        std::vector<uint8_t> pixels;
    };

    namespace gl330 {
        struct texture {
            texture() = default;
            uint32_t        id = 0;
            uint32_t        target = 0;
        };

        inline bool operator==(const texture& lhs, const texture& rhs) {
            return lhs.id == rhs.id;
        }

        auto create_texture_2d(const texture_info &info) -> texture;
        auto create_texture_2d(const image_data &data, const uint32_t flags) -> texture;
        auto create_texture_array() -> texture;
        auto create_texture_cube(const texture_info (&infos)[6]) -> texture;
        auto create_texture_cube(const image_data (&datas)[6], const uint32_t flags) -> texture;

        auto destroy_texture(texture &tex) -> void;
    } // namespace gl330
} // namespace video
