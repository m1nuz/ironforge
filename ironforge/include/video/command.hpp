#pragma once

#include <cstdint>
#include <ironforge_common.hpp>

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
            bind_sampler,
            bind_buffer,
            bind_vertex_array,

            send_uniform
        };

        struct command {
            command_type type;

            union {
                struct {
                    int32_t x;
                    int32_t y;
                    int32_t w;
                    int32_t h;
                } _viewport;

                struct {
                    uint32_t mode;
                    uint32_t type;
                    uint32_t count;
                    uint32_t primcount;
                    uint32_t base_vertex;
                    uint32_t indices_offset;
                } _draw_elements;

                struct {
                    uint32_t id;
                } _bind_framebuffer;

                struct {
                    uint32_t pid;
                } _bind_program;

                struct {
                    uint32_t unit;
                    uint32_t target;
                    uint32_t texture;
                    int32_t location;
                } _bind_texture;

                struct {
                    uint32_t unit;
                    uint32_t sampler;
                } _bind_sampler;

                struct {
                    uint32_t array;
                } _bind_vertex_array;

                struct {
                    int32_t location;
                    uint32_t type;
                    uint32_t size;
                    uint32_t offset;
                    uint32_t count;
                    const void *ptr;
                } _send_uniform;
            };

            command() {
            }

            command(command_type _type) : type(_type) {
            }
        };

    } // namespace gl330
} // namespace video
