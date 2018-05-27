#pragma once

#include <cstdint>
#include <vector>

namespace video {

    struct instance_type;
    typedef instance_type instance_t;

    namespace gl330 {

        /*enum class framebuffer_target : uint32_t {
            draw_framebuffer,
            read_framebuffer,
            framebuffer
        };*/

        enum class framebuffer_attachment : uint32_t {
            color0,
            color1,
            color2,
            color3,
            color4,
            color5,
            color6,
            color7,
            // can be more color_attachment
            depth,
            stencil,
            depth_stencil
        };

        enum class framebuffer_attachment_target : uint32_t {
            texture,
            //texture_2d
            //texture_3d
            renderbuffer,

            //cube_map_positive_x,
            //cube_map_positive_y,
            //cube_map_positive_z,
            //cube_map_negative_x,
            //cube_map_negative_y,
            //cube_map_negative_z
        };

        enum class framebuffer_mask : uint32_t {
            color_buffer = 0x00004000,
            depth_buffer = 0x00000100,
            stencil_buffer = 0x00000400
        };

        constexpr uint32_t INVALID_FRAMEBUFFER_ID = -1;
        constexpr uint32_t DEFAULT_FRAMEBUFFER_ID = 0;

        struct framebuffer_attachments {
            //framebuffer_target target;
            framebuffer_attachment attachment;
            framebuffer_attachment_target attachment_target;
            uint32_t          texture;
            //int32_t level;
            //int32_t layer;
        };
        struct framebuffer_info {
            uint32_t width;
            uint32_t height;
            uint32_t mask;
            std::vector<framebuffer_attachments> attachments;
        };

        struct framebuffer {
            uint32_t id;
            uint32_t width;
            uint32_t height;
            uint32_t mask;
        };

        auto create_framebuffer(const framebuffer_info &info) -> framebuffer;
        auto destroy_framebuffer(framebuffer &buf) -> void;

        auto default_framebuffer(instance_t &vi) -> framebuffer;

    } // namespace gl330

} // namespace video
