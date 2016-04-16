#pragma once

#include <cstdint>

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
        texture_info(pixel_format _format, int32_t _mipmaps, uint32_t _flags, int32_t w, int32_t h, int32_t d, void *_pixels)
            : format{_format}, mipmaps{_mipmaps}, flags{_flags}, width{w}, height{h}, depth{d}, pixels{_pixels} {
        }

        pixel_format format = pixel_format::unknown;
        int32_t     mipmaps = 0;
        uint32_t    flags = 0;

        int32_t     width = 0;
        int32_t     height = 0;
        int32_t     depth = 0;

        void        *pixels = nullptr;
    };

    namespace gl330 {
        struct texture {
            /*texture() : texture{0, 0, 0, 0, 0, 0} {

            }

            texture(uint32_t _id, uint32_t _target, uint32_t _flags, int32_t _width, int32_t _height, int32_t _depth)
                : id{_id}, target{_target}, flags{_flags}, width(_width), height{_height}, depth{_depth} {

            }*/

            uint32_t id;
            uint32_t target;
            uint32_t flags;

            int32_t width;
            int32_t height;
            int32_t depth;
        };

        auto create_texture_2d(const texture_info &info) -> texture;
        auto create_texture_2d(const image_data &data) -> texture;
        auto create_texture_array() -> texture;
        auto create_texture_cube(const texture_info (&infos)[6]) -> texture;
        auto create_texture_cube(const image_data (&datas)[6]) -> texture;

        auto destroy_texture(texture &tex) -> void;
    } // namespace gl330
} // namespace video
