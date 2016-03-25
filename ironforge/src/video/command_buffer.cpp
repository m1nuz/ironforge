#include <glcore_330.h>
#include <core/application.hpp>
#include <video/video.hpp>
#include <video/command_buffer.hpp>

namespace video {
    namespace gl330 {
        command_buffer::command_buffer() : command_buffer{0} {

        }

        command_buffer::command_buffer(size_t mem_size) {
            if (static_cast<size_t>(video::max_uniform_components * 4) < mem_size)
                application::error(application::log_category::video, "No uniforms memory % needed %\n", video::max_uniform_components, mem_size);

            memory = malloc(mem_size);
            memory_size = mem_size;
            memory_offset = 0;
        }

        command_buffer::~command_buffer() {
            free(memory);
            memory = nullptr;
            memory_size = 0;
            memory_offset = 0;
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

        command_buffer& operator <<(command_buffer &cb, const send_uniform &c) {
            auto cc = c;
            if (cb.memory) {
                size_t sz = get_uniform_type_size(c._send_uniform.type) * c._send_uniform.count;
                memcpy((char*)cb.memory + cb.memory_offset, c._send_uniform.ptr, c._send_uniform.size);
                cc._send_uniform.offset = cb.memory_offset;
                cb.memory_offset += sz;
            }

            cb.commands.push_back(cc);
            return cb;
        }

        inline auto dispath_uniform(command_buffer &buf, uint32_t offset, int32_t location, uint32_t type, uint32_t count) -> void {
            switch (type) {
            case GL_UNSIGNED_INT:
                glUniform1uiv(location, count, (const uint32_t*)((const char*)buf.memory + offset));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(location, count, GL_FALSE, (const float*)((const char*)buf.memory + offset));
                break;
            case GL_FLOAT:
                glUniform1fv(location, count, (const float*)((const char*)buf.memory + offset));
                break;
            case GL_FLOAT_VEC2:
                glUniform2fv(location, count, (const float*)((const char*)buf.memory + offset));
                break;
            case GL_FLOAT_VEC3:
                glUniform3fv(location, count, (const float*)((const char*)buf.memory + offset));
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(location, count, (const float*)((const char*)buf.memory + offset));
                break;
            default:
                application::warning(application::log_category::video, "Unknown uniform %", type);
                break;
            }
        }

        auto dispath_command(const command &c, command_buffer &buf) -> void {
            switch (c.type) {
            case command_type::clear:
                glClearColor(buf.clear_color.x, buf.clear_color.y, buf.clear_color.z, buf.clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            case command_type::draw_elements:
                // TODO: use glDrawElementsBaseVertex
                glDrawElements(GL_TRIANGLES, c._draw_elements.count, GL_UNSIGNED_SHORT, nullptr);
                break;
            case command_type::bind_framebuffer:
                glBindFramebuffer(GL_FRAMEBUFFER, c._bind_framebuffer.id);
                break;
            case command_type::bind_program:
                glUseProgram(c._bind_program.pid);
                break;
            case command_type::bind_vertex_array:
                glBindVertexArray(c._bind_vertex_array.array);
                break;
            case command_type::send_uniform:
                dispath_uniform(buf, c._send_uniform.offset, c._send_uniform.location, c._send_uniform.type, c._send_uniform.count);
                break;
            default:
                application::warning(application::log_category::video, "Unknown command %\n", static_cast<uint32_t>(c.type));
                break;
            }
        }
    } // namespace gl330
} // namespace video
