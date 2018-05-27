#pragma once

#include <cstdint>
#include <variant>

#include <core/math.hpp>
#include <video/framebuffer.hpp>
#include <video/program.hpp>
#include <video/texture.hpp>
#include <video/sampler.hpp>
#include <video/vertex_array.hpp>
#include <video/vertices.hpp>

namespace video {

    namespace gl330 {
        // clear
        // bind
        // draw
        // copy
        /*enum class command_type: uint32_t {
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

            send_uniform,

            update,

            blit
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
                    uint32_t mode;
                    uint32_t first;
                    uint32_t count;
                    uint32_t primcount;
                } _draw_arrays;

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

                struct {
                    uint32_t    buf;
                    uint32_t    target;
                    intptr_t    offset;
                    size_t      size;
                    const void* data;
                } _subdata;

                struct {
                    uint32_t src_id;
                    uint32_t dst_id;
                    uint32_t src_x0;
                    uint32_t src_y0;
                    uint32_t src_x1;
                    uint32_t src_y1;
                    uint32_t dst_x0;
                    uint32_t dst_y0;
                    uint32_t dst_x1;
                    uint32_t dst_y1;
                    uint32_t mask;
                    uint32_t filter;
                } _blit;
            };

            command() {
            }

            command(command_type _type) : type(_type) {
            }
        };*/

        namespace detail {
            struct clear {
                clear() = default;
            };

            struct viewport {
                viewport() = default;
                viewport(const framebuffer &fb) : x{0}, y{0}, w{static_cast<int32_t>(fb.width)}, h{static_cast<int32_t>(fb.height)} {

                }

                int32_t x;
                int32_t y;
                int32_t w;
                int32_t h;
            };

            struct draw_elements {
                draw_elements() = default;
                
                draw_elements(const vertices_draw &vd) : mode{vd.mode}, count{vd.count}, primcount{1}, base_vertex{0}, indices_offset{0} {
                    
                }

                uint32_t mode = 0;
                uint32_t type = 0;
                uint32_t count = 0;
                uint32_t primcount = 1;
                uint32_t base_vertex = 0;
                uint32_t indices_offset = 0;
            };

            struct draw_arrays {
                draw_arrays() = default;

                draw_arrays(const vertices_draw &vd) : mode{vd.mode}, count{vd.count}, primcount{1}, first{0} {

                }

                uint32_t mode;
                uint32_t count;
                uint32_t primcount;
                uint32_t first;
            };

            struct bind_framebuffer {
                bind_framebuffer() = default;
                bind_framebuffer(const framebuffer &fb) : id{fb.id} {

                }

                uint32_t id;
            };

            struct bind_program {
                bind_program() = default;
                bind_program(const program &p) : id{p.pid} {

                }

                uint32_t id;
            };

            struct bind_texture {
                bind_texture() = default;

                bind_texture(const program &p, const std::string &name, uint32_t _unit, const texture &tex)
                    : unit{_unit}, target{tex.target}, texture{tex.id}, location{get_uniform_location(p, name)} {

                }

                uint32_t unit;
                uint32_t target;
                uint32_t texture;
                int32_t location;
            };

            struct bind_sampler {
                bind_sampler() = default;
                bind_sampler(const uint32_t _unit, const sampler &s) : unit{_unit}, id{s.id} {

                }

                uint32_t unit;
                uint32_t id;
            };

            struct bind_vertex_array {
                bind_vertex_array() = default;
                bind_vertex_array(const vertex_array &arr) : id{arr.id} {

                }

                uint32_t id;
            };

            struct bind_uniform {
                bind_uniform() = default;

                bind_uniform(const program &p, const std::string &name, const glm::mat4 &m);
                bind_uniform(const program &p, const std::string &name, const glm::mat3 &m);
                bind_uniform(const program &p, const std::string &name, const float &value);
                bind_uniform(const program &p, const std::string &name, const glm::vec2 &value);
                bind_uniform(const program &p, const std::string &name, const glm::vec3 &value);
                bind_uniform(const program &p, const std::string &name, const glm::vec4 &value);
                bind_uniform(const program &p, const std::string &name, const int &value);

                int32_t location;
                uint32_t type;
                uint32_t size;
                uint32_t offset;
                uint32_t count;
                const void *ptr;
            };

            struct update {
                update() = default;
                update(const buffer &b, intptr_t _offset, const void *_data, size_t _size)
                    : buf{b.id}, target{b.target}, offset{_offset}, size{_size}, data{_data} {

                }

                uint32_t    buf;
                uint32_t    target;
                intptr_t    offset;
                size_t      size;
                const void* data;
            };

            struct blit {
                blit() = default;

                blit(const framebuffer &src, const framebuffer &dst);

                uint32_t src_id;
                uint32_t dst_id;
                uint32_t src_x0;
                uint32_t src_y0;
                uint32_t src_x1;
                uint32_t src_y1;
                uint32_t dst_x0;
                uint32_t dst_y0;
                uint32_t dst_x1;
                uint32_t dst_y1;
                uint32_t mask;
                uint32_t filter;
            };
        } // namespace detail

        typedef std::variant<detail::clear, detail::viewport, detail::draw_elements, detail::draw_arrays, detail::bind_framebuffer, detail::bind_program, detail::bind_texture, detail::bind_sampler, detail::bind_vertex_array, detail::bind_uniform, detail::update, detail::blit> command;

    } // namespace gl330

} // namespace video
