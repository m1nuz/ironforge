#include <glcore_330.h>
#include <video/commands.hpp>

namespace video {

    namespace commands {

        /*uniform::uniform(const gl::program &p, const std::string &name, const glm::mat4 &m) : command{gl::command_type::send_uniform} {
            _send_uniform.ptr = &m[0][0];
            _send_uniform.location = gl::get_uniform_location(p, name);
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof m;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_MAT4;
        }

        uniform::uniform(const gl::program &p, const std::string &name, const glm::mat3 &m) : command{gl::command_type::send_uniform} {
            _send_uniform.ptr = &m[0][0];
            _send_uniform.location = gl::get_uniform_location(p, name);
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof m;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_MAT3;
        }

        uniform::uniform(const gl::program &p, const std::string &name, const float &value) : command{gl::command_type::send_uniform} {
            _send_uniform.ptr = &value;
            _send_uniform.location = gl::get_uniform_location(p, name);
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT;
        }

        uniform::uniform(const gl::program &p, const std::string &name, const glm::vec2 &value) : command{gl::command_type::send_uniform} {
            _send_uniform.ptr = &value[0];
            _send_uniform.location = gl::get_uniform_location(p, name);
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_VEC2;
        }

        uniform::uniform(const gl::program &p, const std::string &name, const glm::vec3 &value) : command{gl::command_type::send_uniform} {
            _send_uniform.ptr = &value[0];
            _send_uniform.location = gl::get_uniform_location(p, name);
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_VEC3;
        }

        uniform::uniform(const gl::program &p, const std::string &name, const glm::vec4 &value) : command{gl::command_type::send_uniform} {
            _send_uniform.ptr = &value[0];
            _send_uniform.location = gl::get_uniform_location(p, name);
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_VEC4;
        }

        uniform::uniform(const gl::program &p, const std::string &name, const int &value) : command{gl::command_type::send_uniform} {
            _send_uniform.ptr = &value;
            _send_uniform.location = gl::get_uniform_location(p, name);
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_INT;
        }

        auto get_uniform_type_size(uint32_t type) -> size_t {
            size_t sz = 0;
            switch (type) {
            case GL_INT:
                sz = sizeof(int);
                break;
            case GL_FLOAT:
                sz = sizeof(float);
                break;
            case GL_FLOAT_VEC2:
                sz = sizeof(float) * 2;
                break;
            case GL_FLOAT_VEC3:
                sz = sizeof(float) * 3;
                break;
            case GL_FLOAT_VEC4:
                sz = sizeof(float) * 4;
                break;
            case GL_FLOAT_MAT3:
                sz = sizeof(float) * 9;
                break;
            case GL_FLOAT_MAT4:
                sz = sizeof(float) * 16;
                break;
            }

            return sz;
        }

        blit::blit(const gl330::framebuffer &src, const gl330::framebuffer &dst) : command{gl::command_type::blit} {
            _blit.src_id = src.id;
            _blit.src_x0 = 0;
            _blit.src_y0 = 0;
            _blit.src_x1 = src.width;
            _blit.src_y1 = src.height;

            _blit.dst_id = dst.id;
            _blit.dst_x0 = 0;
            _blit.dst_y0 = 0;
            _blit.dst_x1 = dst.width;
            _blit.dst_y1 = dst.height;

            _blit.mask = GL_COLOR_BUFFER_BIT;
            _blit.filter = GL_LINEAR;
        }

        update::update(gl330::buffer &b, intptr_t offset, const void *data, size_t size) : command{gl::command_type::update} {
            _subdata.buf = b.id;
            _subdata.target = b.target;
            _subdata.offset = offset;
            _subdata.data = data;
            _subdata.size = size;
        }*/

    } // namespace commands

    namespace gl330 {
        /*command_buffer& operator <<(command_buffer &cb, const commands::uniform &c) {
            auto cc = c;
            if (cb.raw_memory) {
                size_t sz = get_uniform_type_size(c._send_uniform.type) * c._send_uniform.count;

                assert(cb.memory_offset + sz < cb.memory_size);

                memcpy((char*)cb.raw_memory + cb.memory_offset, c._send_uniform.ptr, c._send_uniform.size);
                cc._send_uniform.offset = cb.memory_offset;
                cb.memory_offset += sz;
            }

            cb.commands.push_back(cc);
            return cb;
        }*/
        auto operator <<(command_buffer &cb, const detail::bind_uniform &c) -> command_buffer& {
            auto cc = c;
            if (cb.raw_memory) {
                size_t sz = get_uniform_type_size(c.type) * c.count;

                assert(cb.memory_offset + sz < cb.memory_size);

                memcpy((char*)cb.raw_memory + cb.memory_offset, c.ptr, c.size);
                cc.offset = cb.memory_offset;
                cb.memory_offset += sz;
            }

            cb.commands.push_back(cc);
            return cb;
        }

    } // namespace gl330

} // namespace video
