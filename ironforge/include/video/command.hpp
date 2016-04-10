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

        struct clear_op : public command {
            clear_op() : command{command_type::clear} {

            }
        };

        struct viewpor_op : public command {
            viewpor_op(int32_t x, int32_t y, int32_t w, int32_t h) : viewpor_op{{x, y, w, h}} {

            }

            viewpor_op(const glm::ivec4 &vp) : command{command_type::viewport} {
                _viewport.x = vp.x;
                _viewport.y = vp.y;
                _viewport.w = vp.z;
                _viewport.h = vp.w;
            }
        };

        struct draw_elements_op : public command {
            draw_elements_op(uint32_t count) : command{command_type::draw_elements} {
                _draw_elements.count = count;
            }
        };

        struct bind_framebuffer_op : public command {
            bind_framebuffer_op(uint32_t id) : command{command_type::bind_framebuffer} {
                _bind_framebuffer.id = id;
            }
        };

        struct bind_program_op : public command {
            bind_program_op(uint32_t pid) : command{command_type::bind_program} {
                _bind_program.pid = pid;
            }
        };

        struct bind_texture_op : public command  {
            bind_texture_op(int32_t location, uint32_t unit, uint32_t target, uint32_t tex) : command{command_type::bind_texture} {
                _bind_texture.location = location;
                _bind_texture.unit = unit;
                _bind_texture.target = target;
                _bind_texture.texture = tex;
            }
        };

        struct bind_sampler_op : public command  {
            bind_sampler_op(uint32_t unit, uint32_t sam) : command{command_type::bind_sampler} {
                _bind_sampler.unit = unit;
                _bind_sampler.sampler = sam;
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
            send_uniform(int location, const float &value);
            send_uniform(int location, const glm::vec2 &value);
            send_uniform(int location, const glm::vec3 &value);
            send_uniform(int location, const glm::vec4 &value);
            send_uniform(int location, int value);
        };
    } // namespace gl330
} // namespace video
