#pragma once

#include <video/video.hpp>
#include <core/journal.hpp>

namespace video {
    namespace commands {
        struct clear : public gl::command {
            clear() : command{gl::command_type::clear} {

            }
        };

        struct viewport : public gl::command {
            viewport(int32_t x, int32_t y, int32_t w, int32_t h) : viewport{glm::ivec4{x, y, w, h}} {

            }

            viewport(const glm::ivec4 &vp) : command{gl::command_type::viewport} {
                _viewport.x = vp.x;
                _viewport.y = vp.y;
                _viewport.w = vp.z;
                _viewport.h = vp.w;
            }

            viewport(const gl::framebuffer &fb) : viewport{glm::ivec4{0, 0, static_cast<int32_t>(fb.width), static_cast<int32_t>(fb.height)}}
            {
            }
        };

        struct bind : public gl::command {
            bind(const gl::framebuffer &fb) : command{gl::command_type::bind_framebuffer} {
                _bind_framebuffer.id = fb.id;
            }

            bind(const gl::program &p) : command{gl::command_type::bind_program} {
                _bind_program.pid = p.pid;
            }

            bind(const gl::program &p, const std::string &name, uint32_t unit, const gl::texture &tex) : command{gl::command_type::bind_texture} {
                using namespace game;

                _bind_texture.location = gl::get_uniform_location(p, name);
                _bind_texture.target = tex.target;
                _bind_texture.unit = unit;
                _bind_texture.texture = tex.id;

                //journal::debug(journal::_VIDEO, "Bind texture %", tex.id);
            }

            bind(uint32_t unit, const gl::sampler &s) : command{gl::command_type::bind_sampler} {
                _bind_sampler.sampler = s.id;
                _bind_sampler.unit = unit;
            }

            bind(const vertices_source &vs) : command{gl::command_type::bind_vertex_array} {
                _bind_vertex_array.array = vs.array.id;
            }
        };

        struct draw_elements : public gl::command {
            draw_elements(const vertices_draw &dr) : command{gl::command_type::draw_elements} {
                _draw_elements.count = dr.count;
                _draw_elements.mode = dr.mode;
                _draw_elements.primcount = 1;
                _draw_elements.base_vertex = 0;
            }
            draw_elements(const vertices_draw &dr, uint32_t count) : command{gl::command_type::draw_elements_instanced} {
                _draw_elements.count = dr.count;
                _draw_elements.mode = dr.mode;
                _draw_elements.primcount = count;
                _draw_elements.base_vertex = 0;
            }
        };

        struct draw_arrays : public gl::command {
            draw_arrays(const vertices_draw &dr) : command{gl::command_type::draw_arrays} {
                _draw_arrays.count = dr.count;
                _draw_arrays.mode = dr.mode;
                _draw_arrays.first = 0;//dr.base_vertex;
                _draw_arrays.primcount = 1;
            }

            draw_arrays(const vertices_draw &dr, uint32_t count) : command{gl::command_type::draw_arrays_instanced} {
                _draw_arrays.count = dr.count;
                _draw_arrays.mode = dr.mode;
                _draw_arrays.first = 0;//dr.base_vertex;
                _draw_arrays.primcount = count;
            }
        };

        struct uniform : public gl::command {
            uniform(const gl::program &p, const std::string &name, const glm::mat4 &m);
            uniform(const gl::program &p, const std::string &name, const glm::mat3 &m);
            uniform(const gl::program &p, const std::string &name, const float &value);
            uniform(const gl::program &p, const std::string &name, const glm::vec2 &value);
            uniform(const gl::program &p, const std::string &name, const glm::vec3 &value);
            uniform(const gl::program &p, const std::string &name, const glm::vec4 &value);
            uniform(const gl::program &p, const std::string &name, const int &value);
        };

        struct update : public gl::command {
            update(gl::buffer &b, intptr_t offset, const void *data, size_t size);
        };

        struct blit : public gl::command {
            blit(const gl::framebuffer &src, const gl::framebuffer &dst);
        };
    } // namespace commands

    namespace gl330 {
        command_buffer& operator <<(command_buffer &cb, const commands::uniform &c);
    } // namespace gl330
} // namespace video

namespace vcs = video::commands;
