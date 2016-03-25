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
            bind_buffer,
            bind_vertex_array,

            send_uniform
        };

        struct command {
            command_type type;

            union {
                struct {
                    uint32_t vb_offset;
                    uint32_t ib_offset;
                    uint32_t count;
                    uint32_t base_vertex;
                    uint32_t base_index;
                } _draw_elements;

                struct {
                    uint32_t pid;
                } _bind_program;

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

        struct clear_op : public command {
            clear_op() : command{command_type::clear} {

            }
        };

        struct draw_elements_op : public command {
            draw_elements_op(uint32_t count) : command{command_type::draw_elements} {
                _draw_elements.count = count;
            }
        };

        struct bind_program_op : public command {
            bind_program_op(uint32_t pid) : command{command_type::bind_program} {
                _bind_program.pid = pid;
            }
        };

        struct bind_vertex_array_op : public command {
            bind_vertex_array_op(uint32_t array) : command{command_type::bind_vertex_array} {
                _bind_vertex_array.array = array;
            }
        };

        struct send_uniform : public command {
            send_uniform(int location, const glm::mat4 &m);
            send_uniform(int location, const glm::mat3 &m);
            send_uniform(int location, float value);
            send_uniform(int location, const glm::vec2 &value);
            send_uniform(int location, const glm::vec3 &value);
            send_uniform(int location, const glm::vec4 &value);
            send_uniform(int location, int value);
        };
    } // namespace gl330
} // namespace video
