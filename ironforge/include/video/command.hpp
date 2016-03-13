#pragma once

#include <cstdint>

namespace video {
    namespace gl330 {
        // clear
        // bind
        // draw
        // copy
        enum class command_type: uint32_t {
            clear,
            viewport,

            draw_arrays,
            draw_arrays_instanced,
            draw_elements,
            draw_elements_instanced,

            bind_framebuffer,
            bind_program,
            bind_texture,
            bind_buffer,
            bind_vertex_array,
        };

        struct command {
            command_type type;

            union {

            };
        };

        struct clear_op : public command {

        };
    } // namespace gl330
} // namespace video
